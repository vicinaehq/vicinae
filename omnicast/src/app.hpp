#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "command-server.hpp"
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
#include <qwindow.h>
#include <stack>
#include "omnicast.hpp"
#include "ui/action-pannel/action-pannel-widget.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/dialog.hpp"
#include "ui/horizontal-loading-bar.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"

#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class View;
class ViewCommandContext;
class ExtensionView;
class CommandContext;

struct ViewSnapshot {
  View *view;
  QString query;
  QString placeholderText;
  QWidget *searchAccessory;
  std::optional<CompleterData> completer;
  ActionPannelWidget::ViewStack actionViewStack;
  std::optional<NavigationStatus> navigationStatus;

  struct {
    OmniIconUrl icon;
    QString title;
  } navigation;
};

struct CommandSnapshot {
  QStack<ViewSnapshot> viewStack;
  CommandContext *command;
};

struct AlertAction {
  enum Style {
    Default,
    Destructive,
    Cancel,
  };

  QString title;
  std::function<void(void)> handler;
  Style style;
};

class AlertHandler {};

struct ConfirmAlertOptions {
  QString title;
  QString message;
  std::optional<OmniIconUrl> iconUrl;
  std::optional<AlertAction> confirmAction;
  std::optional<AlertAction> cancelAction;
};

class AppWindow : public QMainWindow, public ICommandHandler {
  Q_OBJECT

  CommandServer *_commandServer;
  QPixmap _wallpaper;

  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override;

  QRect viewGeometry() {
    auto windowSize = size();

    return {0, Omnicast::TOP_BAR_HEIGHT, windowSize.width(),
            windowSize.height() - Omnicast::TOP_BAR_HEIGHT - Omnicast::STATUS_BAR_HEIGHT};
  }

public:
  QWidget *centerView;
  std::stack<ViewSnapshot> navigationStack;
  QStack<CommandSnapshot> commandStack;

  void popToRoot();
  void disconnectView(View &view);
  void connectView(View &view);
  void closeWindow(bool popToRoot = false);

  void selectPrimaryAction();
  void selectSecondaryAction();
  void clearSearch();

  TopBar *topBar = nullptr;
  StatusBar *statusBar = nullptr;
  ActionPannelWidget *actionPannel = nullptr;
  QVBoxLayout *layout = nullptr;
  QWidget *defaultWidget = new QWidget();
  HorizontalLoadingBar *_loadingBar;
  DialogWidget *_dialog = nullptr;
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
  void pushView(View *view, const PushViewOptions &opts = {});
  void popCurrentView();
  void executeAction(AbstractAction *action);

signals:
  void currentViewPoped();
  void actionExecuted(AbstractAction *action) const;
};
