#pragma once
#include "global-bar.hpp"
#include "header.hpp"
#include "navigation-controller.hpp"
#include <qevent.h>
#include <qmainwindow.h>
#include <qstackedwidget.h>
#include <qwidget.h>

class LauncherWindow : public QMainWindow {

public:
  LauncherWindow();

protected:
  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *event) override;

private:
  NavigationController m_navigation;
  GlobalHeader *m_header;
  GlobalBar *m_bar = new GlobalBar(m_navigation);
  QStackedWidget *m_currentViewWrapper = new QStackedWidget;

  void handleViewChange(const NavigationController::ViewState &state);
  void setupUI();
  QWidget *createWidget() const;
};
