#pragma once
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "global-bar.hpp"
#include "header.hpp"
#include "navigation-controller.hpp"
#include "ui/dialog.hpp"
#include <qevent.h>
#include <qmainwindow.h>
#include <qstackedwidget.h>
#include <qwidget.h>

class LauncherWindow : public QMainWindow {

public:
  LauncherWindow(ApplicationContext &context);

protected:
  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *event) override;
  void handleActionVisibilityChanged(bool visible);
  void resizeEvent(QResizeEvent *event) override { QMainWindow::resizeEvent(event); }

private:
  ApplicationContext &m_ctx;
  ActionPanelV2Widget *m_actionPanel = new ActionPanelV2Widget;
  GlobalHeader *m_header;
  HDivider *m_barDivider = new HDivider;
  GlobalBar *m_bar = new GlobalBar(m_ctx);
  QStackedWidget *m_currentView = new QStackedWidget;
  QWidget *m_mainWidget = new QWidget(this);
  QStackedWidget *m_currentViewWrapper = new QStackedWidget;
  QStackedWidget *m_currentOverlayWrapper = new QStackedWidget(this);
  DialogWidget *m_dialog = new DialogWidget(this);

  void handleDialog(DialogContentWidget *alert);
  void handleViewChange(const NavigationController::ViewState &state);
  void setupUI();
  QWidget *createWidget() const;
};
