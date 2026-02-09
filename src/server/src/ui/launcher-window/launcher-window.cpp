#include "launcher-window.hpp"
#include "action-panel/action-panel.hpp"
#include "layout.hpp"
#include "services/window-manager/window-manager.hpp"
#include "services/toast/toast-service.hpp"
#include "services/background-effect/background-effect-manager.hpp"
#include "config/config.hpp"
#include "environment.hpp"
#include "keyboard/keybind-manager.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "ui/status-bar/status-bar.hpp"
#include "service-registry.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "ui/top-bar/top-bar.hpp"
#include "utils.hpp"
#include <qcoreevent.h>
#include <qlineedit.h>
#include <qpixmapcache.h>
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
  auto &cfg = m_ctx.services->config()->value();

  m_hud->hide();
  m_ctx.navigation->closeActionPanel();
  tryCenter();
  applyWindowConfig(cfg.launcherWindow);
  tryCompaction();
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
  QPoint center(screen->geometry().center().x() - (width() / 2),
                screen->geometry().center().y() - (height() / 2));
  move(center);
}

LauncherWindow::LauncherWindow(ApplicationContext &ctx)
    : m_ctx(ctx), m_hud(new HudWidget), m_header(new GlobalHeader(*ctx.navigation)),
      m_bar(new GlobalBar(ctx)), m_actionPanel(new ActionPanelV2Widget(this)),
      m_dialog(new DialogWidget(this)), m_currentView(new QStackedWidget(this)),
      m_currentViewWrapper(new QStackedWidget(this)), m_currentOverlayWrapper(new QStackedWidget(this)),
      m_mainWidget(new QWidget(this)), m_barDivider(new HDivider(this)), m_hudDismissTimer(new QTimer(this)) {
  using namespace std::chrono_literals;

  setWindowTitle(Omnicast::MAIN_WINDOW_NAME);

  m_hud->hide();
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
              m_actionPanel->show();
            } else {
              m_actionPanel->close();
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
    if (m_currentOverlayWrapper->isVisible() || m_compacted) return;
    m_bar->setVisible(value);
    m_barDivider->setVisible(value);
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

void LauncherWindow::mouseMoveEvent(QMouseEvent *event) {
  // Not sure how reliable this is. When layer shell is enabled, some compositors send mouse events when
  // the mouse goes outside of the layer surface, so this may fix it.
  if (m_closeOnFocusLoss && !centralWidget()->rect().contains(event->pos())) {
    m_ctx.navigation->closeWindow();
  }

  QMainWindow::mouseMoveEvent(event);
}

void LauncherWindow::setCompacted(bool value) {
  if (m_compacted == value) return;

  auto &cfg = m_ctx.services->config()->value();

  if (value) {
    centralWidget()->setFixedHeight(cfg.header.height);
    m_bar->hide();
    m_currentViewWrapper->hide();
    m_header->setLoadingBarVisibility(false);
    m_barDivider->hide();
  } else {
    centralWidget()->setFixedHeight(QWIDGETSIZE_MAX);
    m_bar->show();
    m_currentViewWrapper->show();
    m_barDivider->show();
    m_header->setLoadingBarVisibility(true);
  }

  m_compacted = value;
  updateBlur();
  update();
}

void LauncherWindow::updateBlur() {
  auto &cfg = m_ctx.services->config()->value().launcherWindow;
  auto wm = m_ctx.services->windowManager();
  auto bgEffectManager = m_ctx.services->backgroundEffectManager();
  int rounding = cfg.clientSideDecorations.enabled ? cfg.clientSideDecorations.rounding : 0;
  QRect blurRegion = rect();

  if (m_compacted) { blurRegion.setHeight(m_header->height()); }

  if (bgEffectManager->supportsBlur()) {
    if (cfg.blur.enabled) {
      bgEffectManager->setBlur(windowHandle(), {.radius = rounding, .region = blurRegion});
    } else {
      bgEffectManager->clearBlur(windowHandle());
    }
  }
}

void LauncherWindow::tryCompaction() {
  auto &cfg = m_ctx.services->config()->value().launcherWindow.compactMode;
  setCompacted(cfg.enabled && m_ctx.navigation->searchText().isEmpty() &&
               m_ctx.navigation->viewStackSize() == 1);
}

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setCentralWidget(createWidget());
  createWinId();
  m_hud->setMaximumWidth(300);
  m_header->input()->installEventFilter(this);
  handleConfigurationChange(m_ctx.services->config()->value());

  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this,
          &LauncherWindow::handleViewChange);
  connect(m_ctx.navigation.get(), &NavigationController::confirmAlertRequested, this,
          &LauncherWindow::handleDialog);

  connect(m_header->input(), &QLineEdit::textChanged, this, [this](const QString &text) { tryCompaction(); });

  connect(m_ctx.services->config(), &config::Manager::configChanged, this,
          [this](const config::ConfigValue &next, const config::ConfigValue &prev) {
            handleConfigurationChange(next);
          });
}

bool LauncherWindow::isCompactable() const {
  return m_ctx.navigation->viewStackSize() == 1 && m_ctx.navigation->searchText().isEmpty();
}

void LauncherWindow::applyWindowConfig(const config::WindowConfig &cfg) {
  auto wm = m_ctx.services->windowManager();
  int rounding = cfg.clientSideDecorations.enabled ? cfg.clientSideDecorations.rounding : 0;

  updateBlur();
  wm->provider()->setDimAround(cfg.dimAround);
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
  int bwidth = value.launcherWindow.clientSideDecorations.borderWidth;

  setMouseTracking(value.closeOnFocusLoss);
  m_closeOnFocusLoss = value.closeOnFocusLoss;
  m_header->setFixedHeight(value.header.height);
  m_bar->setFixedHeight(value.footer.height);
  applyWindowConfig(value.launcherWindow);
  QPixmapCache::setCacheLimit(value.pixmapCacheMb * 1024);

  auto &size = value.launcherWindow.size;
  setFixedSize(QSize{size.width, size.height});
  m_mainWidget->layout()->setContentsMargins(QMargins{bwidth, bwidth, bwidth, bwidth});
  tryCenter();
  tryCompaction();
  update();
}

void LauncherWindow::handleDialog(DialogContentWidget *alert) {
  m_dialog->setContent(alert);
  m_ctx.navigation->closeActionPanel();
  m_dialog->showDialog();
  QTimer::singleShot(0, [this]() { m_dialog->setFocus(); });
}

void LauncherWindow::handleViewChange(const NavigationController::ViewState &state) {
  tryCompaction();
  if (m_dialog->isVisible()) { m_dialog->hide(); }
  if (auto current = m_currentViewWrapper->widget(0)) { m_currentViewWrapper->removeWidget(current); }

  m_currentViewWrapper->addWidget(state.sender);

  if (state.supportsSearch) { m_header->input()->setFocus(); }
}

void LauncherWindow::resizeEvent(QResizeEvent *event) {
  QMainWindow::resizeEvent(event);
  updateBlur();
}

bool LauncherWindow::eventFilter(QObject *watched, QEvent *event) {
  if (watched == m_header->input() && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (m_compacted) {
      if (Keyboard::Shortcut(keyEvent) == Keybind::ToggleActionPanel || keyEvent->key() == Qt::Key_Up ||
          keyEvent->key() == Qt::Key_Down) {
        setCompacted(false);
        return false;
      }

      if (keyEvent->key() == Qt::Key_Return) {
        setCompacted(false);
        return true;
      }
    }
  }

  return false;
}

bool LauncherWindow::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    const auto kb = KeybindManager::instance();
    auto &cfg = m_ctx.services->config()->value();

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
      if (cfg.escapeKeyBehavior == "close_window") {
        m_ctx.navigation->closeWindow();
      } else {
        m_ctx.navigation->goBack();
      }
      return true;
    }

// mostly to help take promotional screenshots - not included in the feature set yet
#ifdef DEBUG
    if (keyEvent == Keyboard::Shortcut(Qt::Key_P, Qt::ControlModifier | Qt::ShiftModifier)) {
      const auto data = grab();

      if (!data.save("/tmp/vicinae.png", "png")) {
        m_ctx.services->toastService()->failure("Failed to take screenshot");
        return true;
      }

      m_ctx.navigation->showHud("Screenshot taken 📸");
      return true;
    }
#endif

    if (AbstractAction *action = m_ctx.navigation->findBoundAction(keyEvent)) {
      m_ctx.navigation->executeAction(action);
      return true;
    }
  }

  return QMainWindow::event(event);
}

void LauncherWindow::paintEvent(QPaintEvent *event) {
  auto &config = m_ctx.services->config()->value();
  const auto &csd = config.launcherWindow.clientSideDecorations;
  QPainter painter(this);
  QColor finalBgColor = OmniPainter::resolveColor(SemanticColor::Background);
  QColor statusColor = OmniPainter::resolveColor(SemanticColor::StatusBarBackground);
  QColor borderColor = OmniPainter::resolveColor(SemanticColor::MainWindowBorder);

  finalBgColor.setAlphaF(config.launcherWindow.opacity);
  statusColor.setAlphaF(config.launcherWindow.opacity);
  painter.setRenderHint(QPainter::Antialiasing, true);

  // helps getting rid of weird opacity artifacting on first launch
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(rect(), Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

  QRect contentRect = m_compacted ? QRect{0, 0, width(), config.header.height} : rect();

  if (config.launcherWindow.clientSideDecorations.enabled) {
    QPainterPath path;

    // draw main rect
    {

      path.addRoundedRect(contentRect, csd.rounding, csd.rounding);
      painter.setPen(QPen(borderColor, csd.borderWidth));
      painter.setClipPath(path);
      painter.fillPath(path, finalBgColor);
      painter.drawPath(path);
    }

    if (m_bar->isVisible()) {
      QRect statusRect = contentRect;
      statusRect.setY(height() - m_bar->height() - config.launcherWindow.clientSideDecorations.borderWidth -
                      10);

      path.clear();
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      path.addRoundedRect(statusRect, csd.rounding, csd.rounding);
      painter.setClipPath(path);
      painter.setPen(Qt::NoPen);
      painter.fillPath(path, statusColor);
      painter.drawPath(path);

      path.clear();
      painter.setCompositionMode(QPainter::CompositionMode_Source);
      QRect missingFill{statusRect.x(), statusRect.y(), statusRect.width(), 10};
      path.addRect(missingFill);
      painter.setPen(Qt::NoPen);
      painter.setClipPath(path);
      painter.fillPath(path, finalBgColor);
      painter.drawPath(path);

      {
        path.clear();
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        path.addRoundedRect(contentRect, csd.rounding, csd.rounding);
        painter.setPen(QPen(borderColor, csd.borderWidth));
        painter.setClipPath(path);
        painter.drawPath(path);
      }
    }
  } else {
    painter.fillRect(contentRect, finalBgColor);
  }
}

QWidget *LauncherWindow::createWidget() const {
  VStack().add(m_header).add(m_currentViewWrapper, 1).add(m_barDivider).add(m_bar).imbue(m_mainWidget);

  m_currentView->addWidget(m_mainWidget);
  m_currentView->addWidget(m_currentOverlayWrapper);

  return m_currentView;
}
