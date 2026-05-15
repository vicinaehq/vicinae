#include "action-panel-controller.hpp"
#include "action-panel-model.hpp"
#include "lib/keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action-panel-view.hpp"
#include "ui/views/base-view.hpp"
#include <QKeyEvent>

ActionPanelController::ActionPanelController(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx) {}

ActionPanelView *ActionPanelController::activeRoot() const {
  return m_activeView ? m_activeView->actionPanelRoot() : nullptr;
}

QString ActionPanelController::primaryActionTitle() const {
  auto *root = activeRoot();
  if (!root) return {};
  auto *primary = root->primaryAction();
  return primary ? primary->title() : QString();
}

QVariantList ActionPanelController::primaryActionShortcutTokens() const {
  auto *root = activeRoot();
  if (!root) return {};
  auto *primary = root->primaryAction();
  if (!primary) return {};
  auto shortcut = primary->shortcut().value_or(Keyboard::Shortcut::enter());
  return shortcut.toDisplayTokens();
}

void ActionPanelController::syncToView(BaseView *view) {
  m_activeView = view;

  auto *root = activeRoot();

  bool const newHasActions = root && root->hasActions();
  bool const newHasMultiple = root && root->hasMultipleActions();

  if (newHasActions != m_hasActions) {
    m_hasActions = newHasActions;
    emit hasActionsChanged();
    if (!m_hasActions) close();
  }
  if (newHasMultiple != m_hasMultipleActions) {
    m_hasMultipleActions = newHasMultiple;
    emit hasMultipleActionsChanged();
  }

  emit primaryActionChanged();
}

void ActionPanelController::toggle() {
  if (m_open) {
    close();
  } else {
    open();
  }
}

void ActionPanelController::open() {
  if (m_open) return;
  if (!m_hasActions) return;

  m_open = true;
  emit openChanged();
  openRootPanel();
}

void ActionPanelController::close() {
  if (!m_open) return;

  m_open = false;
  emit openChanged();
  emit stackClearRequested();
  m_depth = 0;
  m_currentPanel = nullptr;

  if (auto *root = activeRoot()) root->onDeactivate();

  delete m_connectionGuard;
  m_connectionGuard = nullptr;

  if (m_activeView) { m_activeView->clearActionPanelStack(); }

  emit depthChanged();
}

void ActionPanelController::openRootPanel() {
  delete m_connectionGuard;
  m_connectionGuard = new QObject(this);

  auto *root = activeRoot();
  if (!root) return;

  root->onActivate();
  connectView(root);

  auto url = root->componentUrl();
  auto props = root->componentProps();
  emit panelPushRequested(url, props);
}

void ActionPanelController::pushPanel(const QUrl &componentUrl, const QVariantMap &properties) {
  emit panelPushRequested(componentUrl, properties);
}

void ActionPanelController::pop() {
  if (m_depth > 1) {
    emit panelPopRequested();
  } else {
    close();
  }
}

void ActionPanelController::onPanelPushed(QObject *panel) {
  m_currentPanel = panel;
  m_depth++;
  emit depthChanged();
}

void ActionPanelController::onPanelPopped(QObject *currentPanel) {
  if (m_depth > 0) m_depth--;
  m_currentPanel = currentPanel;
  if (m_activeView) m_activeView->popActionPanelView();
  emit depthChanged();
}

bool ActionPanelController::tryShortcut(int key, int modifiers) {
  if (!m_open || !m_currentPanel) return false;

  auto val = m_currentPanel->property("boundActions");
  if (!val.isValid()) return false;

  auto *model = qobject_cast<ActionPanelModel *>(val.value<QObject *>());
  if (!model) return false;

  return model->activateByShortcut(key, modifiers);
}

bool ActionPanelController::executePrimaryAction() {
  auto *root = activeRoot();
  if (!root) return false;
  auto *primary = root->primaryAction();
  if (!primary) return false;
  executeAction(primary);
  return true;
}

void ActionPanelController::executeAction(AbstractAction *action) {
  m_ctx.navigation->executeAction(action);
  close();
}

void ActionPanelController::connectView(ActionPanelView *view) {
  connect(view, &ActionPanelView::actionExecuted, m_connectionGuard, [this](AbstractAction *action) {
    m_ctx.navigation->executeAction(action);
    close();
  });

  connect(view, &ActionPanelView::closeRequested, m_connectionGuard, [this]() { close(); });

  connect(view, &ActionPanelView::pushViewRequested, m_connectionGuard, [this](ActionPanelView *child) {
    if (!m_activeView) return;
    m_activeView->pushActionPanelView(child);
    connectView(child);
    auto url = child->componentUrl();
    auto props = child->componentProps();
    emit panelPushRequested(url, props);
  });
}
