#pragma once
#include "app-database.hpp"
#include "calculator-database.hpp"
#include "clipboard-service.hpp"
#include "extend/image-model.hpp"
#include "extension_manager.hpp"
#include "indexer-service.hpp"
#include "process-manager-service.hpp"
#include "quicklist-database.hpp"
#include <jsoncpp/json/value.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qhash.h>
#include <stack>

#include "icon-cache-service.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/list-view.hpp"
#include "ui/status_bar.hpp"
#include "ui/test-list.hpp"
#include "ui/top_bar.hpp"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

template <class T> using Service = T &;

class View;
class ViewCommand;
class ExtensionView;
class Command;

struct ViewSnapshot {
  View *view;
  QString query;
  QString placeholderText;
  QList<AbstractAction *> actions;
  std::optional<CompleterData> completer;
};

struct CommandSnapshot {
  QStack<ViewSnapshot> viewStack;
  ViewCommand *command;
};

struct NavigationStatus {
  QString title;
  ImageLikeModel icon;
};

struct LaunchCommandOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

struct PushViewOptions {
  QString searchQuery;
  std::optional<NavigationStatus> navigation;
};

class AppWindow : public QMainWindow {
  Q_OBJECT

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  std::stack<QString> queryStack;

  std::stack<ViewSnapshot> navigationStack;
  QStack<CommandSnapshot> commandStack;

  std::unique_ptr<QuicklistDatabase> quicklinkDatabase;
  std::unique_ptr<CalculatorDatabase> calculatorDatabase;
  std::unique_ptr<ClipboardService> clipboardService;
  std::unique_ptr<AppDatabase> appDb;
  std::unique_ptr<ExtensionManager> extensionManager;
  std::unique_ptr<IconCacheService> iconCache;
  std::unique_ptr<IndexerService> indexer;
  std::unique_ptr<ProcessManagerService> processManagerService;

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
  ActionPopover *actionPopover = nullptr;
  QVBoxLayout *layout = nullptr;
  QWidget *defaultWidget = new QWidget();

  void popToRoot();

  void launchCommand(ViewCommand *cmd, const LaunchCommandOptions &opts = {});

  AppWindow(QWidget *parent = 0);
  // bool eventFilter(QObject *obj, QEvent *event) override;
  bool event(QEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
  void pushView(View *view, const PushViewOptions &opts = {});
  void popCurrentView();
  void executeAction(AbstractAction *action);

signals:
  void currentViewPoped();
};

struct OpenAppAction : public AbstractAction {
  std::shared_ptr<DesktopExecutable> application;
  QList<QString> args;

  void execute(AppWindow &app) override {
    if (!app.appDb->launch(*application.get(), args)) {
      app.statusBar->setToast("Failed to start app", ToastPriority::Danger);
      return;
    }

    app.closeWindow(true);
  }

  OpenAppAction(const std::shared_ptr<DesktopExecutable> &app, const QString &title,
                const QList<QString> args)
      : AbstractAction(title, ThemeIconModel{.iconName = app->iconName()}), application(app), args(args) {}
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
      : AbstractAction(title, ThemeIconModel{.iconName = ":icons/copy-clipboard"}), text(text) {}
};

class CopyCalculatorResultAction : public CopyTextAction {
  CalculatorItem item;

public:
  void execute(AppWindow &app) override {
    app.calculatorDatabase->saveComputation(item.expression, QString::number(item.result));
    CopyTextAction::execute(app);
  }

  CopyCalculatorResultAction(const CalculatorItem &item, const QString &title, const QString &copyText)
      : CopyTextAction(title, copyText), item(item) {}
};

class StandardListItem : public AbstractNativeListItem {
  QString title;
  QString subtitle;
  QString kind;
  ImageLikeModel imageLike;

  QWidget *createItem() const override {
    return new ListItemWidget(ImageViewer::createFromModel(imageLike, {25, 25}), title, "", kind);
  }

  QWidget *updateItem(QWidget *current) const override {
    auto widget = static_cast<ListItemWidget *>(current);

    return createItem();
  }

  int height() const override { return 40; }
  int role() const override { return 0; }

public:
  StandardListItem(const QString &title, const QString &subtitle, const QString &kind,
                   const ImageLikeModel &imageLike, size_t id = qHash(QUuid::createUuid()))
      : AbstractNativeListItem(id), title(title), subtitle(subtitle), kind(kind), imageLike(imageLike) {}
};

class AppListItem : public StandardListItem {
  std::shared_ptr<DesktopEntry> app;
  Service<AppDatabase> appDb;

  QList<AbstractAction *> createActions() const override {
    QList<AbstractAction *> actions;
    auto fileBrowser = appDb.defaultFileBrowser();
    auto textEditor = appDb.defaultTextEditor();

    actions << new OpenAppAction(app, "Open", {});

    for (const auto &desktopAction : app->actions) {
      actions << new OpenAppAction(desktopAction, desktopAction->name, {});
    }

    if (fileBrowser) { actions << new OpenAppAction(fileBrowser, "Open in folder", {app->path}); }

    if (textEditor) { actions << new OpenAppAction(textEditor, "Open desktop file", {app->path}); }

    actions << new CopyTextAction("Copy file path", app->path);

    return actions;
  }

  size_t id() const override { return qHash(app->id); }

public:
  AppListItem(const std::shared_ptr<DesktopEntry> &app, Service<AppDatabase> appDb)
      : StandardListItem(app->name, "", "Application", ThemeIconModel{.iconName = app->iconName()}), app(app),
        appDb(appDb) {}
  ~AppListItem() { qDebug() << "destroy app list item"; }
};
