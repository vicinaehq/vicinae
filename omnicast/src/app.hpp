#pragma once
#include "ai/ai-provider.hpp"
#include "app/app-database.hpp"
#include "calculator-database.hpp"
#include "clipboard/clipboard-service.hpp"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "command-database.hpp"
#include "command-server.hpp"
#include <QScreen>
#include "extend/image-model.hpp"
#include "extension_manager.hpp"
#include "indexer-service.hpp"
#include "omni-icon.hpp"
#include "process-manager-service.hpp"
#include "proto.hpp"
#include "quicklist-database.hpp"
#include <QtWaylandClient/qwaylandclientextension.h>
#include <cstring>
#include <qboxlayout.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qgraphicseffect.h>
#include <qgraphicsscene.h>
#include <qhash.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qscreen_platform.h>
#include <stack>

#include "ui/action-pannel/action-pannel-widget.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/dialog.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <wayland-client-protocol.h>

template <class T> using Service = T &;

class View;
class ViewCommandContext;
class ExtensionView;
class CommandContext;

class ViewDisplayer : public QWidget {
  QVBoxLayout *layout;

public:
  ViewDisplayer() : layout(new QVBoxLayout) {
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }

  void setWidget(QWidget *widget) {
    layout->takeAt(0);
    layout->addWidget(widget);
  }
};

struct ViewSnapshot {
  View *view;
  QString query;
  QString placeholderText;
  std::optional<CompleterData> completer;
  ActionPannelWidget::ViewStack actionViewStack;
};

struct CommandSnapshot {
  QStack<ViewSnapshot> viewStack;
  CommandContext *command;
};

struct NavigationStatus {
  QString title;
  OmniIconUrl iconUrl;
};

struct LaunchCommandOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

struct PushViewOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

class AppWindow : public QMainWindow, public ICommandHandler {
  Q_OBJECT

  CommandServer *_commandServer;
  QPixmap _wallpaper;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void recomputeWallpaper() {
    QPixmap canva(size());
    QPixmap pix("/home/aurelle/Downloads/sequoia.jpg");
    QPainter cp(&canva);

    cp.drawPixmap(0, 0, pix.scaled(size(), Qt::KeepAspectRatioByExpanding));
    _wallpaper = blurPixmap(canva);
  }

  QPixmap blurPixmap(const QPixmap &source) {
    if (source.isNull()) return {};

    QPixmap result = source;
    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;

    blur->setBlurRadius(50); // Adjust radius as needed
    blur->setBlurHints(QGraphicsBlurEffect::PerformanceHint);

    // Apply the blur using QGraphicsScene
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(source);
    item.setGraphicsEffect(blur);
    scene.addItem(&item);

    // Render the result
    result = QPixmap(source.size());
    result.fill(Qt::transparent);
    QPainter painter(&result);
    scene.render(&painter);

    delete blur;
    return result;
  }

  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override;

public:
  std::stack<ViewSnapshot> navigationStack;
  QStack<CommandSnapshot> commandStack;

  std::unique_ptr<QuicklistDatabase> quicklinkDatabase;
  std::unique_ptr<CalculatorDatabase> calculatorDatabase;
  std::unique_ptr<ClipboardService> clipboardService;
  std::unique_ptr<AbstractAppDatabase> appDb;
  std::unique_ptr<ExtensionManager> extensionManager;
  std::unique_ptr<ProcessManagerService> processManagerService;
  std::unique_ptr<CommandDatabase> commandDb;
  std::unique_ptr<AbstractAiProvider> aiProvider;

  void popToRootView();
  void disconnectView(View &view);
  void connectView(View &view);
  void closeWindow(bool popToRoot = false);

  void selectPrimaryAction();
  void selectSecondaryAction();
  void clearSearch();

  template <typename T> Service<T> service() const;

  TopBar *topBar = nullptr;
  StatusBar *statusBar = nullptr;
  ActionPannelWidget *actionPannel = nullptr;
  QVBoxLayout *layout = nullptr;
  QWidget *defaultWidget = new QWidget();
  ViewDisplayer *viewDisplayer;
  HorizontalLoadingBar *_loadingBar;
  DialogWidget *_dialog = nullptr;
  AlertWidget *_alert = new AlertWidget;

  void popToRoot();

  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchCommandOptions &opts = {});

  AppWindow(QWidget *parent = 0);
  bool event(QEvent *event) override;

public slots:
  void pushView(View *view, const PushViewOptions &opts = {});
  void popCurrentView();
  void executeAction(AbstractAction *action);

signals:
  void currentViewPoped();
  void actionExecuted(AbstractAction *action) const;
};

struct OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;

  void execute(AppWindow &app) override {
    if (!app.appDb->launch(*application.get(), args)) {
      app.statusBar->setToast("Failed to start app", ToastPriority::Danger);
      return;
    }

    app.closeWindow(true);
  }

  QString id() const override { return application->id(); }

  OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                const std::vector<QString> args)
      : AbstractAction(title, app->iconUrl()), application(app), args(args) {}
};

class CopyTextAction : public AbstractAction {
  QString text;

public:
  void execute(AppWindow &app) override {
    app.clipboardService->copyText(text);
    app.closeWindow();
    app.statusBar->setToast("Copied in clipboard");
  }

  CopyTextAction(const QString &title, const QString &text)
      : AbstractAction(title, BuiltinOmniIconUrl("copy-clipboard")), text(text) {}
};

class CopyCalculatorResultAction : public CopyTextAction {
  CalculatorItem item;

public:
  void execute(AppWindow &app) override {
    app.calculatorDatabase->insertComputation(item.expression, QString::number(item.result));
    CopyTextAction::execute(app);
  }

  CopyCalculatorResultAction(const CalculatorItem &item, const QString &title, const QString &copyText)
      : CopyTextAction(title, copyText), item(item) {}
};
