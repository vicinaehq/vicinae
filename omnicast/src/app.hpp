#pragma once
#include "app-database.hpp"
#include "calculator-database.hpp"
#include "clipboard/clipboard-service.hpp"
#include "command-database.hpp"
#include "command-server.hpp"
#include <QScreen>
#include "extend/image-model.hpp"
#include "extension_manager.hpp"
#include "get-wayland-wlr-data-control-client-protocol.hpp"
#include "indexer-service.hpp"
#include "omni-icon.hpp"
#include "process-manager-service.hpp"
#include "proto.hpp"
#include "quicklist-database.hpp"
#include <QtWaylandClient/qwaylandclientextension.h>
#include <cstring>
#include <qboxlayout.h>
#include <qevent.h>
#include <qhash.h>
#include <qlogging.h>
#include <qobject.h>
#include <qscreen_platform.h>
#include <stack>

#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"
#include "wayland-wlr-data-control-client-protocol.h"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <stdexcept>
#include <wayland-client-protocol.h>

template <class T> using Service = T &;

class View;
class ViewCommand;
class ExtensionView;
class Command;

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
  QList<AbstractAction *> actions;
  std::optional<CompleterData> completer;
};

struct CommandSnapshot {
  QStack<ViewSnapshot> viewStack;
  ViewCommand *command;
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

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void showEvent(QShowEvent *event) override {
    QPoint globalCursorPos = QCursor::pos();
    QPoint localPos = mapFromGlobal(globalCursorPos);

    // Create and send a mouse move event
    QMouseEvent mouseEvent(QEvent::MouseMove, localPos, globalCursorPos, Qt::NoButton, Qt::NoButton,
                           Qt::NoModifier);
    QApplication::sendEvent(this, &mouseEvent);
    qDebug() << "showing!!";

    QMainWindow::showEvent(event);
  }

  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override {
    if (message.type == "ping") { return "pong"; }
    if (message.type == "toggle") {
      setVisible(!isVisible());
      return true;
    }

    if (message.type == "clipboard.store") {
      auto values = message.params.asArray();

      if (values.size() != 2) { return CommandError{"Ill-formed clipboard.store request"}; }

      auto data = values[0].asString();
      auto options = values[1].asDict();
      auto mimeName = options["mime"].asString();
      auto copied = clipboardService->copy(QByteArray(data.data(), data.size()));

      return copied;
    }

    if (message.type == "command.list") {
      Proto::Array results;

      for (const auto &cmd : commandDb->list()) {
        Proto::Dict result;

        result["id"] = cmd.id.toUtf8().constData();
        result["name"] = cmd.name.toUtf8().constData();
        results.push_back(result);
      }

      return results;
    }

    if (message.type == "command.push") {
      auto args = message.params.asArray();

      if (args.empty()) { return CommandError{"Ill-formed command.push request"}; }

      auto id = args.at(0).asString();

      if (auto cmd = commandDb->findById(id.c_str())) {
        emit launchCommand(cmd->factory(*this, ""),
                           {.navigation = NavigationStatus{.title = cmd->name, .iconUrl = cmd->iconUrl}});
        setVisible(true);

        return true;
      }

      return CommandError{"No such command"};
    }

    return CommandError{"Unknowm command"};
  }

public:
  std::stack<QString> queryStack;

  std::stack<ViewSnapshot> navigationStack;
  QStack<CommandSnapshot> commandStack;

  std::unique_ptr<QuicklistDatabase> quicklinkDatabase;
  std::unique_ptr<CalculatorDatabase> calculatorDatabase;
  std::unique_ptr<ClipboardService> clipboardService;
  std::unique_ptr<AppDatabase> appDb;
  std::unique_ptr<ExtensionManager> extensionManager;
  std::unique_ptr<ProcessManagerService> processManagerService;
  std::unique_ptr<CommandDatabase> commandDb;

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
  ViewDisplayer *viewDisplayer;

  void popToRoot();

  void launchCommand(ViewCommand *cmd, const LaunchCommandOptions &opts = {});

  AppWindow(QWidget *parent = 0);
  bool event(QEvent *event) override;

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
