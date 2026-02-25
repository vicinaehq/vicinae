#include "action-panel-controller.hpp"
#include "action-panel-model.hpp"
#include "navigation-controller.hpp"
#include "lib/keyboard/keyboard.hpp"
#include <QKeyEvent>

ActionPanelController::ActionPanelController(ApplicationContext &ctx, QObject *parent)
    : QObject(parent), m_ctx(ctx) {}

QString ActionPanelController::primaryActionTitle() const {
  return m_primary ? m_primary->title() : QString();
}

QString ActionPanelController::primaryActionShortcut() const {
  if (!m_primary) return {};
  auto shortcut = m_primary->shortcut().value_or(Keyboard::Shortcut::enter());
  return shortcut.toDisplayString();
}

void ActionPanelController::setStateFrom(const ActionPanelState &state) {
  m_allActions.clear();
  m_sections.clear();
  m_primary = nullptr;

  for (const auto &section : state.sections()) {
    SectionSnapshot snap;
    snap.name = section->name();
    for (const auto &action : section->actions()) {
      snap.actions.push_back(action);
      m_allActions.push_back(action);
    }
    m_sections.push_back(std::move(snap));
  }

  m_primary = state.primaryAction();

  bool newHasActions = state.actionCount() > 0;
  bool newHasMultiple = state.actionCount() > 1;

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

void ActionPanelController::clearState() {
  m_allActions.clear();
  m_sections.clear();
  m_primary = nullptr;

  if (m_hasActions) {
    m_hasActions = false;
    emit hasActionsChanged();
  }
  if (m_hasMultipleActions) {
    m_hasMultipleActions = false;
    emit hasMultipleActionsChanged();
  }

  emit primaryActionChanged();
  close();
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
  emit depthChanged();
  destroyPanelModels();
}

void ActionPanelController::destroyPanelModels() {
  if (m_rootModel) {
    m_rootModel->deleteLater();
    m_rootModel = nullptr;
  }
}

void ActionPanelController::openRootPanel() {
  destroyPanelModels();

  auto state = std::make_unique<ActionPanelState>();
  for (const auto &snap : m_sections) {
    auto *section = state->createSection(snap.name);
    for (const auto &action : snap.actions) {
      section->m_actions.push_back(action);
    }
  }
  state->finalize();

  m_rootModel = new ActionPanelModel(std::move(state), this);
  connectModel(m_rootModel);

  QVariantMap props;
  props[QStringLiteral("model")] = QVariant::fromValue(static_cast<QObject *>(m_rootModel));
  emit panelPushRequested(QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")), props);
}

void ActionPanelController::pushActionList(std::unique_ptr<ActionPanelState> state) {
  if (!state) return;

  state->finalize();
  auto *model = new ActionPanelModel(std::move(state), m_rootModel);
  connectModel(model);

  QVariantMap props;
  props[QStringLiteral("model")] = QVariant::fromValue(static_cast<QObject *>(model));
  emit panelPushRequested(QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")), props);
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
  emit depthChanged();
}

AbstractAction *ActionPanelController::findBoundAction(const QKeyEvent *event) const {
  for (const auto &action : m_allActions) {
    if (action->isBoundTo(event)) return action.get();
  }

  return nullptr;
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
  if (!m_primary) return false;
  executeAction(m_primary);
  return true;
}

void ActionPanelController::executeAction(AbstractAction *action) {
  m_ctx.navigation->executeAction(action);
  close();
}

void ActionPanelController::connectModel(ActionPanelModel *model) {
  connect(model, &ActionPanelModel::actionExecuted, this, [this](AbstractAction *action) {
    m_ctx.navigation->executeAction(action);
    close();
  });

  connect(model, &ActionPanelModel::submenuRequested, this, [this](ActionPanelModel *subModel) {
    connectModel(subModel);
    QVariantMap props;
    props[QStringLiteral("model")] = QVariant::fromValue(static_cast<QObject *>(subModel));
    emit panelPushRequested(QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")), props);
  });

  connect(model, &ActionPanelModel::closeRequested, this, [this]() { close(); });
}
