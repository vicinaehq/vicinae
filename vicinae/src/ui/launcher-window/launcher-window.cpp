#include "launcher-window.hpp"
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "services/window-manager/window-manager.hpp"
#include "config/config.hpp"
#include "environment.hpp"
#include "keyboard/keybind-manager.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/status-bar/status-bar.hpp"
#include "service-registry.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/top-bar/top-bar.hpp"
#include "utils.hpp"
#include <qcoreevent.h>
#include <qscreen.h>
#include <QStackedWidget>
#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/Window>
#endif
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qwidget.h>
#include "vicinae.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/image/url.hpp"
#include "ui/search-bar/search-bar.hpp"
#include "ui/dialog/dialog.hpp"
#include "ui/overlay/overlay.hpp"
#include "ui/hud/hud.hpp"
#include "ui/views/base-view.hpp"
#include "settings-controller/settings-controller.hpp"

void LauncherWindow::showEvent(QShowEvent *event) {
  m_hud->hide();
  m_ctx.navigation->closeActionPanel();
  tryCenter();
  QWidget::showEvent(event);
  activateWindow(); // gnome needs this
}

void LauncherWindow::tryCenter() {
  QScreen *targetScreen = screen();

  if (auto name = m_ctx.services->config()->value().launcherWindow.screen; name != "auto") {
    auto screens = QApplication::screens();
    if (auto it = std::ranges::find_if(
            screens, [&](const QScreen *s) { return s->name() == qStringFromStdView(name); });
        it != screens.end()) {
      targetScreen = *it;
    }
  }

  centerOnScreen(targetScreen);
}

void LauncherWindow::centerOnScreen(const QScreen *screen) {
  QPoint center(screen->geometry().center().x() - width() / 2,
                screen->geometry().center().y() - height() / 2);
  move(center);
}

LauncherWindow::LauncherWindow(ApplicationContext &ctx) : m_ctx(ctx) {
  using namespace std::chrono_literals;

  setWindowTitle(Omnicast::MAIN_WINDOW_NAME);

  m_hud = new HudWidget;
  m_header = new GlobalHeader(*m_ctx.navigation);
  m_bar = new GlobalBar(m_ctx);
  m_actionPanel = new ActionPanelV2Widget(this);
  m_dialog = new DialogWidget(this);
  m_currentView = new QStackedWidget(this);
  m_currentViewWrapper = new QStackedWidget(this);
  m_currentOverlayWrapper = new QStackedWidget(this);
  m_mainWidget = new QWidget(this);
  m_barDivider = new HDivider(this);
  m_hudDismissTimer = new QTimer(this);
  m_actionVeil = new ActionVeilWidget(this);

  m_hud->hide();
  m_actionVeil->hide();
  m_hudDismissTimer->setInterval(1500ms);
  m_hudDismissTimer->setSingleShot(true);
  m_dialog->hide();

  setupUI();

  connect(m_actionPanel, &ActionPanelV2Widget::openChanged, this, [this](bool opened) {
    if (opened) {
      m_ctx.navigation->openActionPanel();
    } else {
      m_ctx.navigation->closeActionPanel();
    }
  });

  connect(m_actionPanel, &ActionPanelV2Widget::actionActivated, this, [this](AbstractAction *action) {
    if (action->isSubmenu()) {
      // For submenu actions, push the submenu view instead of executing
      auto submenuView = action->createSubmenu();
      if (submenuView) {
        m_actionPanel->pushView(submenuView);
        // Keep the panel open for submenus
        return;
      }
    }

    // For regular actions, execute and close the panel
    m_ctx.navigation->executeAction(action);
    m_ctx.navigation->closeActionPanel();
  });

  connect(m_ctx.navigation.get(), &NavigationController::submenuRequested, this,
          [this](ActionPanelView *view) {
            if (!view) return;
            m_actionPanel->pushView(view);
          });

  connect(m_hudDismissTimer, &QTimer::timeout, this, [this]() { m_hud->hide(); });

  connect(m_ctx.navigation.get(), &NavigationController::actionPanelVisibilityChanged, this,
          [this](bool value) {
            if (value) {
              m_focusWidget = QApplication::focusWidget();
              m_actionVeil->setFixedSize(size());
              m_actionVeil->move(0, 0);
              m_actionVeil->raise();
              m_actionVeil->show();
              m_actionPanel->show();
            } else {
              m_actionVeil->close();
              m_actionPanel->close();
              if (m_focusWidget) {
                QTimer::singleShot(0, [this]() {
                  m_focusWidget->setFocus();
                  m_focusWidget = nullptr;
                });
              }
            }
          });

  connect(m_ctx.navigation.get(), &NavigationController::showHudRequested, this,
          &LauncherWindow::handleShowHUD);

  connect(m_ctx.overlay.get(), &OverlayController::currentOverlayDismissed, this, [this]() {
    if (auto widget = m_currentOverlayWrapper->widget(0)) { m_currentOverlayWrapper->removeWidget(widget); }

    m_currentView->setCurrentWidget(m_mainWidget);
    m_header->input()->setFocus();
  });

  connect(m_ctx.overlay.get(), &OverlayController::currentOverlayChanged, this, [this](OverlayView *overlay) {
    m_currentOverlayWrapper->addWidget(overlay);
    m_currentView->setCurrentWidget(m_currentOverlayWrapper);
    overlay->setFocus();
  });

  connect(m_ctx.navigation.get(), &NavigationController::actionsChanged, this,
          [this](auto &&actions) { m_actionPanel->setNewActions(actions); });

  connect(m_ctx.navigation.get(), &NavigationController::windowVisiblityChanged, this, [this](bool visible) {
    setVisible(visible);
    if (!visible) {
      auto &cfg = m_ctx.services->config()->value();
      setFixedSize(QSize{cfg.launcherWindow.size.width, cfg.launcherWindow.size.height});
    }
  });
  connect(m_ctx.navigation.get(), &NavigationController::windowSizeRequested, this,
          [this](QSize size) { setFixedSize(size); });

  connect(m_ctx.navigation.get(), &NavigationController::headerVisiblityChanged, this, [this](bool value) {
    if (m_currentOverlayWrapper->isVisible()) return;
    m_header->setVisible(value);
  });

  connect(m_ctx.navigation.get(), &NavigationController::searchVisibilityChanged, [this](bool visible) {
    m_header->input()->setVisible(visible);
    if (visible && !m_currentOverlayWrapper->isVisible()) m_header->input()->setFocus();
  });
  connect(m_ctx.navigation.get(), &NavigationController::statusBarVisiblityChanged, this, [this](bool value) {
    if (m_currentOverlayWrapper->isVisible()) return;
    m_bar->setVisible(value);
  });
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { repaint(); });
}

void LauncherWindow::handleShowHUD(const QString &text, const std::optional<ImageURL> &icon) {
  m_hud->clear();
  m_hud->setText(text);
  if (icon) m_hud->setIcon(*icon);
  m_hud->show();
  m_hudDismissTimer->start();
}

void LauncherWindow::hideEvent(QHideEvent *event) {
  m_ctx.navigation->closeWindow();
  QWidget::hideEvent(event);
}

void LauncherWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::ActivationChange) { m_ctx.navigation->setWindowActivated(isActiveWindow()); }

  QWidget::changeEvent(event);
}

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setCentralWidget(createWidget());
  createWinId();
  m_hud->setMaximumWidth(300);

  handleConfigurationChange(m_ctx.services->config()->value());

  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this,
          &LauncherWindow::handleViewChange);
  connect(m_ctx.navigation.get(), &NavigationController::confirmAlertRequested, this,
          &LauncherWindow::handleDialog);
  connect(m_actionVeil, &ActionVeilWidget::mousePressed, this,
          [this]() { m_ctx.navigation->closeActionPanel(); });

  connect(m_ctx.services->config(), &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            handleConfigurationChange(next);
          });
}

void LauncherWindow::handleConfigurationChange(const config::ConfigValue &value) {
#ifdef WAYLAND_LAYER_SHELL
  const auto &lc = value.launcherWindow.layerShell;
  if (Environment::isLayerShellSupported() && lc.enabled) {
    namespace Shell = LayerShellQt;
    using clsh = config::LayerShellConfig;

    if (auto lshell = Shell::Window::get(windowHandle())) {
      lshell->setLayer(lc.layer == "overlay" ? Shell::Window::LayerOverlay : Shell::Window::LayerTop);
      lshell->setScope(Omnicast::LAYER_SCOPE);
      lshell->setScreenConfiguration(Shell::Window::ScreenFromCompositor);
      lshell->setAnchors(Shell::Window::AnchorNone);
      lshell->setKeyboardInteractivity(lc.keyboardInteractivity == "exclusive"
                                           ? Shell::Window::KeyboardInteractivityExclusive
                                           : Shell::Window::KeyboardInteractivityOnDemand);
    }
  }
#endif

  auto wm = m_ctx.services->windowManager();
  auto &blurCfg = value.launcherWindow.blur;

  wm->provider()->setBlur({.enabled = blurCfg.enabled});

  m_header->setFixedHeight(value.header.height);
  m_bar->setFixedHeight(value.footer.height);

  auto &size = value.launcherWindow.size;
  setFixedSize(QSize{size.width, size.height});
  tryCenter();

  update();
}

void LauncherWindow::handleDialog(DialogContentWidget *alert) {
  m_dialog->setContent(alert);
  m_ctx.navigation->closeActionPanel();
  m_dialog->showDialog();
  QTimer::singleShot(0, [this]() { m_dialog->setFocus(); });
}

void LauncherWindow::handleViewChange(const NavigationController::ViewState &state) {
  if (m_dialog->isVisible()) { m_dialog->hide(); }
  if (auto current = m_currentViewWrapper->widget(0)) { m_currentViewWrapper->removeWidget(current); }

  m_currentViewWrapper->addWidget(state.sender);

  if (state.supportsSearch) { m_header->input()->setFocus(); }
}

bool LauncherWindow::event(QEvent *event) {
  auto kb = KeybindManager::instance();

  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (kb->resolve(Keybind::ToggleActionPanel) == keyEvent) {
      m_ctx.navigation->toggleActionPanel();
      return true;
    }

    if (keyEvent == Keyboard::Shortcut(Qt::Key_Escape, Qt::ShiftModifier)) {
      m_ctx.navigation->popToRoot();
      return true;
    }

    if (kb->resolve(Keybind::OpenSettings) == keyEvent) {
      m_ctx.navigation->closeWindow();
      m_ctx.settings->openWindow();
      return true;
    }

    if (keyEvent == Keyboard::Shortcut(Qt::Key_Escape)) {
      m_ctx.navigation->goBack();
      return true;
    }

    if (AbstractAction *action = m_ctx.navigation->findBoundAction(keyEvent)) {
      m_ctx.navigation->executeAction(action);
      return true;
    }
  }

  return QMainWindow::event(event);
}

void LauncherWindow::handleActionVisibilityChanged(bool visible) {
  if (visible) {
    m_actionPanel->show();
    return;
  }

  m_actionPanel->hide();
}

void LauncherWindow::paintEvent(QPaintEvent *event) {
  auto &config = m_ctx.services->config()->value();
  OmniPainter painter(this);
  QColor finalBgColor = painter.resolveColor(SemanticColor::Background);

  finalBgColor.setAlphaF(config.launcherWindow.opacity);
  painter.setRenderHint(QPainter::Antialiasing, true);

  if (config.launcherWindow.clientSideDecorations.enabled) {
    QPainterPath path;
    path.addRoundedRect(rect(), config.launcherWindow.clientSideDecorations.rounding,
                        config.launcherWindow.clientSideDecorations.rounding);
    painter.setClipPath(path);
    painter.fillPath(path, finalBgColor);
    painter.setThemePen(SemanticColor::MainWindowBorder,
                        config.launcherWindow.clientSideDecorations.borderWidth);
    painter.drawPath(path);
  } else {
    painter.fillRect(rect(), finalBgColor);
  }
}

QWidget *LauncherWindow::createWidget() const {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(2, 2, 2, 2);
  layout->setSpacing(0);
  layout->addWidget(m_header);
  layout->addWidget(m_currentViewWrapper, 1);
  layout->addWidget(m_barDivider);
  layout->addWidget(m_bar);
  m_mainWidget->setLayout(layout);

  m_currentView->addWidget(m_mainWidget);
  m_currentView->addWidget(m_currentOverlayWrapper);

  return m_currentView;
}
