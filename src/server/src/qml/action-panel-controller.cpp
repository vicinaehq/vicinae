#include "action-panel-controller.hpp"
#include "action-panel-model.hpp"
#include "internal/keyboard/keyboard.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/action-list-view.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/action-pannel/action-panel-view.hpp"
#include "ui/views/base-view.hpp"
#include <QKeyEvent>

ActionPanelController::ActionPanelController(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx) {}

bool ActionPanelController::hasActions() const {
  auto *r = activeRoot();
  return r && r->hasActions();
}

bool ActionPanelController::hasMultipleActions() const {
  auto *r = activeRoot();
  return r && r->hasMultipleActions();
}

ActionPanelView *ActionPanelController::activeRoot() const {
  if (m_ownedRoot) return m_ownedRoot.get();
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

void ActionPanelController::setActions(std::unique_ptr<ActionPanelState> actions) {
  close();

  auto *prevRoot = activeRoot();
  if (prevRoot) prevRoot->onUnmount();

  m_activeView = nullptr;
  m_ownedRoot = std::make_unique<ActionListView>(this);
  m_ownedRoot->adoptState(std::move(actions));
  m_ownedRoot->onMount();

  emit actionsChanged();
  emit primaryActionChanged();
}

void ActionPanelController::syncToView(BaseView *view) {
  auto *prevRoot = activeRoot();
  auto prevOwned = std::move(m_ownedRoot);

  m_activeView = view;

  auto *root = activeRoot();

  if (prevRoot != root) {
    if (prevRoot) prevRoot->onUnmount();
    if (root) root->onMount();
  }

  emit actionsChanged();
  if (!hasActions()) close();

  emit primaryActionChanged();

  refreshSubmenus();
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
  if (!hasActions()) return;

  m_open = true;
  emit openChanged();
  openRootPanel();
}

void ActionPanelController::close() {
  if (!m_open) return;

  auto *root = activeRoot();

  m_open = false;
  emit openChanged();
  emit stackClearRequested();
  m_currentPanel = nullptr;

  if (root) root->onDeactivate();

  delete m_connectionGuard;
  m_connectionGuard = nullptr;

  clearSubmenuStack();

  emit depthChanged();
}

void ActionPanelController::openRootPanel() {
  delete m_connectionGuard;
  m_connectionGuard = new QObject(this);

  auto *root = activeRoot();
  if (!root) return;

  root->resetState();
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
  if (!m_submenuStack.empty()) {
    emit panelPopRequested();
  } else {
    close();
  }
}

void ActionPanelController::onPanelPushed(QObject *panel) {
  m_currentPanel = panel;
  emit depthChanged();
}

void ActionPanelController::onPanelPopped(QObject *currentPanel) {
  m_currentPanel = currentPanel;

  if (!m_submenuStack.empty()) {
    auto *view = m_submenuStack.back();
    view->onUnmount();
    view->deleteLater();
    m_submenuStack.pop_back();
  }

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

bool ActionPanelController::activateBoundAction(const QKeyEvent *event) {
  auto *root = activeRoot();
  if (!root) return false;

  auto *action = root->findBoundAction(event);
  if (!action) return false;

  if (auto *submenu = dynamic_cast<SubmenuAction *>(action)) {
    openSubmenu(submenu);
  } else {
    executeAction(action);
  }

  return true;
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

void ActionPanelController::openSubmenu(SubmenuAction *action) {
  open();

  auto *root = dynamic_cast<ActionListView *>(activeRoot());
  if (root) root->activateSubmenu(action);
}

void ActionPanelController::connectView(ActionPanelView *view) {
  connect(view, &ActionPanelView::actionExecuted, m_connectionGuard, [this](AbstractAction *action) {
    m_ctx.navigation->executeAction(action);
    close();
  });

  connect(view, &ActionPanelView::closeRequested, m_connectionGuard, [this]() { close(); });

  connect(view, &ActionPanelView::pushViewRequested, m_connectionGuard, [this](ActionPanelView *child) {
    child->setParent(this);
    m_submenuStack.push_back(child);
    child->onMount();
    connectView(child);
    auto url = child->componentUrl();
    auto props = child->componentProps();
    emit panelPushRequested(url, props);
  });
}

void ActionPanelController::clearSubmenuStack() {
  for (auto it = m_submenuStack.rbegin(); it != m_submenuStack.rend(); ++it) {
    (*it)->onUnmount();
    (*it)->deleteLater();
  }
  m_submenuStack.clear();
}

void ActionPanelController::refreshSubmenus() {
  if (m_submenuStack.empty()) return;

  auto *root = dynamic_cast<ActionListView *>(activeRoot());
  if (!root || !root->state()) return;

  const auto *parentState = root->state();

  for (auto *view : m_submenuStack) {
    auto viewId = view->id();
    if (viewId.isEmpty()) break;

    auto *submenuAction = parentState->findSubmenuAction(viewId);
    if (!submenuAction) break;

    auto submenuState = submenuAction->createSubmenuStateStealthily();
    if (!submenuState) break;

    parentState = submenuState.get();
    static_cast<ActionListView *>(view)->adoptState(std::move(submenuState));
  }
}
