#include "launcher-window.hpp"
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "ui/keyboard.hpp"
#include "ui/status-bar/status-bar.hpp"
#include "ui/top-bar/top-bar.hpp"
#include "utils/environment.hpp"
#include <qtenvironmentvariables.h>
#ifdef WAYLAND_LAYER_SHELL
#include <LayerShellQt/window.h>
#endif
#include <qboxlayout.h>
#include <qevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qstackedwidget.h>
#include <qwidget.h>
#include "../image/url.hpp"
#include "vicinae.hpp"
#include "services/config/config-service.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "root-command.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/dialog/dialog.hpp"
#include "ui/overlay/overlay.hpp"
#include "ui/hud/hud.hpp"
#include "ui/views/base-view.hpp"
#include <QStackedWidget>
#include "settings-controller/settings-controller.hpp"

void LauncherWindow::showEvent(QShowEvent *event) { m_hud->hide(); }

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

  m_header->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
  m_bar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);
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
    m_ctx.navigation->executeAction(action);
    m_ctx.navigation->closeActionPanel();
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
              m_actionVeil->hide();
              m_actionPanel->hide();
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

  connect(m_ctx.navigation.get(), &NavigationController::windowVisiblityChanged, this,
          [this](bool visible) { setVisible(visible); });

  ctx.navigation->pushView(new RootSearchView);
  ctx.navigation->setNavigationIcon(ImageURL::builtin("vicinae"));

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

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);
  setCentralWidget(createWidget());

  m_hud->setMaximumWidth(300);

#ifdef WAYLAND_LAYER_SHELL
  if (Environment::isLayerShellEnabled()) {
    namespace Shell = LayerShellQt;

    createWinId();
    if (auto lshell = Shell::Window::get(windowHandle())) {
      lshell->setLayer(Shell::Window::LayerOverlay);
      lshell->setScope("vicinae");
      lshell->setScreenConfiguration(Shell::Window::ScreenFromCompositor);
      lshell->setKeyboardInteractivity(Shell::Window::KeyboardInteractivityExclusive);
      lshell->setExclusiveZone(-1);
      lshell->setAnchors(Shell::Window::AnchorNone);
    } else {

      qWarning()
          << "Unable apply layer shell rules to main window: LayerShellQt::Window::get() returned null";
    }
  } else {
    qInfo() << "USE_LAYER_SHELL=0 is set, not using layer shell";
  }
#endif

  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this,
          &LauncherWindow::handleViewChange);
  connect(m_ctx.navigation.get(), &NavigationController::confirmAlertRequested, this,
          &LauncherWindow::handleDialog);
  connect(m_actionVeil, &ActionVeilWidget::mousePressed, this,
          [this]() { m_ctx.navigation->closeActionPanel(); });
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
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_B)) {
      m_ctx.navigation->openActionPanel();
      return true;
    }

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ShiftModifier, Qt::Key_Escape)) {
      m_ctx.navigation->popToRoot();
      return true;
    }

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_Comma)) {
      m_ctx.navigation->closeWindow();
      m_ctx.settings->openWindow();
      return true;
    }

    switch (keyEvent->key()) {
    case Qt::Key_Escape: {
      if (m_ctx.navigation->viewStackSize() == 1) {
        if (m_ctx.navigation->searchText().isEmpty()) {
          m_ctx.navigation->closeWindow();
          return true;
        }

        m_ctx.navigation->clearSearchText();
        return true;
      }

      m_ctx.navigation->popCurrentView();
      return true;
    }
    default:
      break;
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
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 2;
  QColor finalBgColor = theme.colors.base00;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  if (config.window.csd) {
    QPainterPath path;
    path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

    painter.setClipPath(path);

    painter.fillPath(path, finalBgColor);

    QPen pen(theme.resolveTint(SemanticColor::Border), borderWidth);
    painter.setPen(pen);

    painter.drawPath(path);
  } else {
    painter.fillRect(rect(), finalBgColor);
  }
}

QWidget *LauncherWindow::createWidget() const {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_header);
  layout->addWidget(m_currentViewWrapper, 1);
  layout->addWidget(m_barDivider);
  layout->addWidget(m_bar, 1);
  m_mainWidget->setLayout(layout);

  m_currentView->addWidget(m_mainWidget);
  m_currentView->addWidget(m_currentOverlayWrapper);

  return m_currentView;
}
