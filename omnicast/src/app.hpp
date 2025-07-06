#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "action-panel/action-panel.hpp"
#include "command-server.hpp"
#include "settings/settings-window.hpp"
#include <QScreen>
#include "omni-icon.hpp"
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
#include <qstackedwidget.h>
#include <qwindow.h>
#include "ui/alert.hpp"
#include "ui/dialog.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/status_bar.hpp"
#include "ui/toast.hpp"
#include "ui/top_bar.hpp"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class View;
class ViewCommandContext;
class ExtensionView;
class CommandContext;
class BaseView;

struct ViewSnapshot {
  BaseView *view;
  QString query;
  QString placeholderText;
  QWidget *searchAccessory;
  std::optional<CompleterData> completer;
  std::optional<NavigationStatus> navigationStatus;

  struct {
    OmniIconUrl icon;
    QString title;
  } navigation;

  ~ViewSnapshot() {}
};

struct CommandSnapshot {
  QStack<ViewSnapshot> viewStack;
  std::unique_ptr<CommandContext> command;
};

class AppWindow : public QMainWindow, public ICommandHandler {
  Q_OBJECT

  SettingsWindow *settings = new SettingsWindow();
  CommandServer *_commandServer;
  std::vector<BaseView *> m_viewStack;

  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QStackedWidget *m_viewContainer = new QStackedWidget(this);

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override;

  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override;

  void unloadHangingCommand();

  void changeEvent(QEvent *event) override {
    if (event->type() == QEvent::WindowActivate) {
      qDebug() << "App Window activated";
    } else if (event->type() == QEvent::WindowDeactivate) {
      qDebug() << "Window deactivated";
    }

    QMainWindow::changeEvent(event);
  }

public:
  StatusBar *m_statusBar = new StatusBar();
  // to compile
  TopBar *topBar = new TopBar(this);
  ActionPanelV2Widget *m_actionPanel = new ActionPanelV2Widget(this);

  HorizontalLoadingBar *_loadingBar = nullptr;
  void setToast(const QString &title, ToastPriority priority = ToastPriority::Success) {}

  std::vector<CommandSnapshot> commandStack;

  void popToRoot();
  void disconnectView(BaseView &view);
  void connectView(BaseView &view);
  void closeWindow(bool popToRoot = false);

  bool replaceView(BaseView *previous, BaseView *next);

  BaseView *frontView() const;
  void presentFrontView();

  void selectPrimaryAction();
  void selectSecondaryAction();
  void clearSearch();

  DialogWidget *_dialog = new DialogWidget(this);
  AlertWidget *_alert = new AlertWidget;

  void launchCommand(const std::shared_ptr<AbstractCmd> &cmd, const LaunchCommandOptions &opts = {},
                     const LaunchProps &props = {});
  void launchCommand(const QString &id, const LaunchCommandOptions &opts = {});
  void unloadCurrentCommand();

  AppWindow(QWidget *parent = 0);

  bool event(QEvent *event) override;

  void confirmAlert(AlertWidget *alert) {
    _dialog->setContent(alert);
    _dialog->showDialog();
    _dialog->setFocus();
  }

public slots:
  void pushView(BaseView *view, const PushViewOptions &opts = {});
  void popCurrentView();

signals:
  void currentViewPoped();
};
