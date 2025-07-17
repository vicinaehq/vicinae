#include "global-bar.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qwidget.h>

NavigationStatusWidget::NavigationStatusWidget() { setupUI(); }

void NavigationStatusWidget::setTitle(const QString &title) { m_navigationTitle->setText(title); }
void NavigationStatusWidget::setIcon(const OmniIconUrl &icon) { m_navigationIcon->setUrl(icon); }

void NavigationStatusWidget::setupUI() {
  auto layout = new QHBoxLayout;

  m_navigationIcon->setFixedSize(20, 20);

  layout->setAlignment(Qt::AlignVCenter);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(10);
  layout->addWidget(m_navigationIcon);
  layout->addWidget(m_navigationTitle);
  setLayout(layout);
}

GlobalBar::GlobalBar(ApplicationContext &ctx) : m_ctx(ctx) { setupUI(); }

void GlobalBar::paintEvent(QPaintEvent *event) { QWidget::paintEvent(event); }

void GlobalBar::handleActionPanelVisiblityChange(bool visible) { m_actionButton->hoverChanged(visible); }

void GlobalBar::actionsChanged(const ActionPanelState &actions) {
  auto primaryAction = actions.findPrimaryAction();

  if (primaryAction) {
    m_primaryActionButton->setText(primaryAction->title());
    m_primaryActionButton->setShortcut(
        primaryAction->shortcut.value_or(KeyboardShortcutModel{.key = "return"}));
  }

  m_primaryActionButton->setVisible(primaryAction);
  m_actionButton->setText("Actions");
  m_actionButton->setVisible(primaryAction);
  m_actionButton->setShortcut(KeyboardShortcutModel{.key = "B", .modifiers = {"ctrl"}});
}

void GlobalBar::handleViewStateChange(const NavigationController::ViewState &state) {}

void GlobalBar::handleToast(const Toast *toast) {
  m_toast->setToast(toast);
  m_leftWidget->setCurrentWidget(m_toast);
}

void GlobalBar::handleToastDestroyed(const Toast *toast) { m_leftWidget->setCurrentWidget(m_status); }

void GlobalBar::setupUI() {
  auto toast = m_ctx.services->toastService();

  setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);
  auto layout = new QHBoxLayout;

  m_primaryActionButton->hide();
  m_actionButton->hide();

  layout->setContentsMargins(15, 5, 15, 5);
  layout->setSpacing(0);
  m_leftWidget->addWidget(m_status);
  m_leftWidget->addWidget(m_toast);
  m_leftWidget->setCurrentWidget(m_status);
  layout->addWidget(m_leftWidget, 0);
  layout->addStretch();
  layout->addWidget(m_primaryActionButton);
  layout->addWidget(m_actionButton);
  m_status->setIcon(BuiltinOmniIconUrl("omnicast"));
  setLayout(layout);

  connect(m_primaryActionButton, &ShortcutButton::clicked, this,
          [this]() { m_ctx.navigation->executePrimaryAction(); });

  connect(m_actionButton, &ShortcutButton::clicked, this, [this]() { m_ctx.navigation->openActionPanel(); });

  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this, [this]() {
    auto state = m_ctx.navigation->topState();
    if (auto &ac = state->actionPanelState) { actionsChanged(*ac.get()); }
  });

  connect(m_ctx.navigation.get(), &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const OmniIconUrl &icon) {
            m_status->setTitle(title);
            m_status->setIcon(icon);
          });

  connect(m_ctx.navigation.get(), &NavigationController::currentViewStateChanged, this,
          &GlobalBar::handleViewStateChange);

  connect(m_ctx.navigation.get(), &NavigationController::currentViewStateChanged, this,
          &GlobalBar::handleViewStateChange);
  connect(m_ctx.navigation.get(), &NavigationController::actionPanelVisibilityChanged, this,
          &GlobalBar::handleActionPanelVisiblityChange);
  connect(m_ctx.navigation.get(), &NavigationController::actionsChanged, this, &GlobalBar::actionsChanged);

  connect(toast, &ToastService::toastActivated, this, &GlobalBar::handleToast);
  connect(toast, &ToastService::toastHidden, this, &GlobalBar::handleToastDestroyed);
}
