#include "status-bar.hpp"
#include "common.hpp"
#include "keyboard/keybind-manager.hpp"
#include "navigation-controller.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "ui/spinner/spinner.hpp"
#include "utils/layout.hpp"
#include "vicinae.hpp"
#include "ui/shortcut-button/shortcut-button.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qsizepolicy.h>
#include <qstackedwidget.h>
#include <qwidget.h>
#include "services/toast/toast-service.hpp"
#include "ui/toast/toast.hpp"
#include "ui/typography/typography.hpp"

NavigationStatusWidget::NavigationStatusWidget() { setupUI(); }

void NavigationStatusWidget::setTitle(const QString &title) { m_navigationTitle->setText(title); }
void NavigationStatusWidget::setIcon(const ImageURL &icon) { m_navigationIcon->setUrl(icon); }

void NavigationStatusWidget::setSuffixIcon(const std::optional<ImageURL> &icon) {
  if (icon) m_suffixIcon->setUrl(*icon);
  m_suffixIcon->setVisible(icon.has_value());
}

void NavigationStatusWidget::setupUI() {
  m_navigationTitle = new TypographyWidget(this);
  m_navigationTitle->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_navigationIcon->setFixedSize(20, 20);
  m_suffixIcon->setFixedSize(20, 20);
  m_suffixIcon->hide();

  HStack()
      .spacing(10)
      .add(m_navigationIcon)
      .add(m_navigationTitle)
      .add(m_suffixIcon)
      .addStretch()
      .imbue(this);
}

GlobalBar::GlobalBar(ApplicationContext &ctx) : m_ctx(ctx) { setupUI(); }

void GlobalBar::handleActionPanelVisiblityChange(bool visible) { m_actionButton->hoverChanged(visible); }

void GlobalBar::actionsChanged(const ActionPanelState &panel) {
  auto primaryAction = panel.primaryAction();

  if (primaryAction) {
    m_primaryActionButton->setText(primaryAction->title());
    m_primaryActionButton->setShortcut(primaryAction->shortcut().value_or(Keyboard::Shortcut::enter()));
  }

  m_primaryActionButton->setVisible(primaryAction);
  m_actionButton->setText("Actions");
  m_actionButton->setVisible(panel.actionCount() > 1);
  m_actionButton->setShortcut(Keybind::ToggleActionPanel);
}

void GlobalBar::handleViewStateChange(const NavigationController::ViewState &state) {}

void GlobalBar::handleToast(const Toast *toast) {
  m_toast->setToast(toast);
  m_leftWidget->setCurrentWidget(m_toast);
}

void GlobalBar::handleToastDestroyed() { m_leftWidget->setCurrentWidget(m_status); }

void GlobalBar::setupUI() {
  auto toast = m_ctx.services->toastService();

  m_leftWidget = new QStackedWidget;
  m_primaryActionButton = new ShortcutButton;
  m_actionButton = new ShortcutButton;
  m_toast = new ToastWidget;
  m_status = new NavigationStatusWidget;

  setFixedHeight(Omnicast::STATUS_BAR_HEIGHT);

  m_primaryActionButton->hide();
  m_actionButton->hide();

  m_leftWidget->addWidget(m_status);
  m_leftWidget->addWidget(m_toast);
  m_leftWidget->setCurrentWidget(m_status);
  m_status->setIcon(ImageURL::builtin("vicinae"));
  setAttribute(Qt::WA_TranslucentBackground);

  // action buttons can never shrink
  m_actionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_primaryActionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  HStack()
      .marginsX(15)
      .marginsY(5)
      .spacing(10)
      .add(m_leftWidget)
      .addStretch()
      .add(HStack().add(m_primaryActionButton).add(m_actionButton))
      .imbue(this);

  connect(m_primaryActionButton, &ShortcutButton::clicked, this,
          [this]() { m_ctx.navigation->executePrimaryAction(); });

  connect(m_actionButton, &ShortcutButton::clicked, this,
          [this]() { m_ctx.navigation->toggleActionPanel(); });

  connect(m_ctx.navigation.get(), &NavigationController::currentViewChanged, this, [this]() {
    auto state = m_ctx.navigation->topState();
    if (auto &ac = state->actionPanelState) { actionsChanged(*ac.get()); }
  });

  connect(m_ctx.navigation.get(), &NavigationController::navigationStatusChanged, this,
          [this](const QString &title, const ImageURL &icon) {
            m_status->setTitle(title);
            m_status->setIcon(icon);
          });

  connect(m_ctx.navigation.get(), &NavigationController::navigationSuffixIconChanged, this,
          [this](const std::optional<ImageURL> &icon) { m_status->setSuffixIcon(icon); });

  connect(KeybindManager::instance(), &KeybindManager::keybindChanged, this,
          [this](const Keybind &bind, const Keyboard::Shortcut &shortcut) {
            if (bind == Keybind::ToggleActionPanel) { m_actionButton->setShortcut(shortcut); }
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
