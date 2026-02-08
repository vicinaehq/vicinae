#pragma once
#include <qcoreevent.h>
#include <qdebug.h>
#include <qevent.h>
#include <qlogging.h>
#include <qmainwindow.h>
#include <qobject.h>
#include <qscreen.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "services/background-effect/abstract-background-effect-manager.hpp"
#include "ui/image/url.hpp"
#include "config/config.hpp"
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

class LauncherWindow : public QMainWindow {

public:
  LauncherWindow(ApplicationContext &context);

protected:
  void paintEvent(QPaintEvent *event) override;
  bool event(QEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void showEvent(QShowEvent *event) override;
  void changeEvent(QEvent *event) override;
  bool eventFilter(QObject *watched, QEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void handleConfigurationChange(const config::ConfigValue &value);
  void applyWindowConfig(const config::WindowConfig &cfg);
  bool isCompactable() const;

  void setCompacted(bool value);
  void tryCompaction();

protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void tryCenter();
  void updateBlur();
  void centerOnScreen(const QScreen *screen);
  void handleShowHUD(const QString &text, const std::optional<ImageURL> &icon);
  void handleDialog(DialogContentWidget *alert);
  void handleViewChange(const NavigationController::ViewState &state);
  void setupUI();
  QWidget *createWidget() const;

private:
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
  bool m_compacted = false;
  bool m_closeOnFocusLoss = false;
};
