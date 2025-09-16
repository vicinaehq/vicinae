#pragma once
#include <qcoreevent.h>
#include <qdebug.h>
#include <qevent.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "../image/url.hpp"
#include "navigation-controller.hpp"

class ApplicationContext;
class QMainWindow;
class ActionPanelV2Widget;
class GlobalHeader;
class HudWidget;
class GlobalBar;
class QStackedWidget;
class HDivider;
class DialogWidget;
class DialogContentWidget;
class HDivider;
class ImageURL;

class ActionVeilWidget : public QWidget {
  Q_OBJECT

public:
  ActionVeilWidget(QWidget *parent) : QWidget(parent) {}

  void mousePressEvent(QMouseEvent *event) override { emit mousePressed(); }

signals:
  void mousePressed() const;
};

class LauncherWindow : public QMainWindow {

public:
  LauncherWindow(ApplicationContext &context);

protected:
  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *event) override;
  void handleActionVisibilityChanged(bool visible);
  void hideEvent(QHideEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void changeEvent(QEvent *event) override;

private:
  ActionVeilWidget *m_actionVeil;
  ApplicationContext &m_ctx;
  ActionPanelV2Widget *m_actionPanel = nullptr;
  GlobalHeader *m_header = nullptr;
  HudWidget *m_hud = nullptr;
  QTimer *m_hudDismissTimer = nullptr;
  HDivider *m_barDivider = nullptr;
  GlobalBar *m_bar = nullptr;
  QStackedWidget *m_currentView = nullptr;
  QWidget *m_mainWidget = nullptr;
  QStackedWidget *m_currentViewWrapper = nullptr;
  QStackedWidget *m_currentOverlayWrapper = nullptr;
  DialogWidget *m_dialog = nullptr;
  QWidget *m_focusWidget = nullptr;

  void tryCenter();

  void handleShowHUD(const QString &text, const std::optional<ImageURL> &icon);
  void handleDialog(DialogContentWidget *alert);
  void handleViewChange(const NavigationController::ViewState &state);
  void setupUI();
  QWidget *createWidget() const;
};
