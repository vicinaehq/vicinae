#pragma once
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include "action-panel/action-panel.hpp"
#include "command-server.hpp"
#include "settings/settings-window.hpp"
#include <QScreen>
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
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/dialog.hpp"
#include "ui/status_bar.hpp"
#include "ui/top_bar.hpp"
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AppWindow : public QMainWindow, public ICommandHandler {
  Q_OBJECT

  StatusBar *m_statusBar = new StatusBar();
  TopBar *m_topBar = new TopBar(this);

  DialogWidget *_dialog = new DialogWidget(this);
  AlertWidget *_alert = new AlertWidget;

  SettingsWindow *settings = new SettingsWindow();
  CommandServer *_commandServer;

  QVBoxLayout *m_layout = new QVBoxLayout(this);
  QStackedWidget *m_viewContainer = new QStackedWidget(this);

  std::variant<CommandResponse, CommandError> handleCommand(const CommandMessage &message) override;
  bool eventFilter(QObject *watched, QEvent *event) override;
  void executeAction(AbstractAction *action);
  void unloadHangingCommand();
  void closeWindow(bool popToRoot = false);
  void confirmAlert(AlertWidget *alert);
  void applyActionPanelState(ActionPanelV2Widget *panel);

  void handleActionButtonClick();
  void handleCurrentActionButtonClick();

  void handleTextEdited(const QString &text);
  void handleTextPop();
  void handleCompleterArgumentsChanged(const std::vector<std::pair<QString, QString>> &arguments);

protected:
  bool event(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

public:
  AppWindow(QWidget *parent = 0);
};
