#include "launcher-window.hpp"
#include "action-panel/action-panel.hpp"
#include "common.hpp"
#include "header.hpp"
#include "navigation-controller.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>
#include "omni-icon.hpp"
#include "omnicast.hpp"
#include "overlay-controller/overlay-controller.hpp"
#include "root-command.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/dialog.hpp"
#include "ui/overlay/overlay.hpp"

LauncherWindow::LauncherWindow(ApplicationContext &ctx)
    : m_ctx(ctx), m_header(new GlobalHeader(*m_ctx.navigation)) {
  m_header->setFixedHeight(Omnicast::TOP_BAR_HEIGHT);
  m_bar->setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);
  setupUI();

  connect(m_actionPanel, &ActionPanelV2Widget::openChanged, this, [this](bool opened) {
    if (opened)
      m_ctx.navigation->openActionPanel();
    else
      m_ctx.navigation->closeActionPanel();
  });

  connect(m_actionPanel, &ActionPanelV2Widget::actionActivated, this, [this](AbstractAction *action) {
    action->execute(&m_ctx);
    m_ctx.navigation->closeActionPanel();
  });

  connect(m_ctx.navigation.get(), &NavigationController::actionPanelVisibilityChanged, this,
          [this](bool value) {
            if (value) {
              m_actionPanel->show();
            } else {
              m_actionPanel->hide();
            }
          });

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
  ctx.navigation->setNavigationIcon(BuiltinOmniIconUrl("omnicast"));

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

void LauncherWindow::setupUI() {
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground, true);
  setMinimumSize(Omnicast::WINDOW_SIZE);
  setCentralWidget(createWidget());
  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this,
          &LauncherWindow::handleViewChange);
  connect(m_ctx.navigation.get(), &NavigationController::confirmAlertRequested, this,
          &LauncherWindow::handleDialog);
}

void LauncherWindow::handleDialog(DialogContentWidget *alert) {
  m_dialog->setContent(alert);
  // we need to make sure no other popup is opened for the dialog to properly
  // show up
  m_actionPanel->close();
  m_dialog->showDialog();
  m_dialog->setFocus();
}

void LauncherWindow::handleViewChange(const NavigationController::ViewState &state) {
  if (m_dialog->isVisible()) { m_dialog->close(); }
  if (auto current = m_currentViewWrapper->widget(0)) { m_currentViewWrapper->removeWidget(current); }

  m_currentViewWrapper->addWidget(state.sender);

  if (state.supportsSearch) { m_header->input()->setFocus(); }
}

bool LauncherWindow::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ControlModifier, Qt::Key_B)) {
      m_actionPanel->show();
      return true;
    }

    if (keyEvent->keyCombination() == QKeyCombination(Qt::ShiftModifier, Qt::Key_Escape)) {
      m_ctx.navigation->popToRoot();
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

    // handle bound actions
    if (auto state = m_ctx.navigation->topState()) {
      if (state->actionPanelState) {
        for (const auto &section : state->actionPanelState->sections()) {
          for (const auto &action : section->actions()) {
            if (action->shortcut && KeyboardShortcut(*action->shortcut) == keyEvent) {
              action->execute(&m_ctx);
              return true;
            }
          }
        }
      }
    }
  }

  return QMainWindow::event(event);
}

void LauncherWindow::handleActionVisibilityChanged(bool visible) {
  if (visible) {
    m_actionPanel->show();
    return;
  }

  m_actionPanel->close();
}

void LauncherWindow::paintEvent(QPaintEvent *event) {
  auto &config = ServiceRegistry::instance()->config()->value();
  auto &theme = ThemeService::instance().theme();
  int borderWidth = 2;
  QColor finalBgColor = theme.colors.mainBackground;
  QPainter painter(this);

  finalBgColor.setAlphaF(config.window.opacity);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), config.window.rounding, config.window.rounding);

  painter.setClipPath(path);

  painter.fillPath(path, finalBgColor);

  QPen pen(theme.colors.border, borderWidth);
  painter.setPen(pen);

  painter.drawPath(path);
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
