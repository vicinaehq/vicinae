#include "ui/action-pannel/action-list-view.hpp"
#include "action-panel-model.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include <QKeyEvent>

ActionListView::ActionListView(QObject *parent)
    : ActionPanelView(parent), m_state(std::make_unique<ActionPanelState>()),
      m_model(new ActionPanelModel(m_state.get(), this)) {
  connect(m_model, &ActionPanelModel::actionExecuted, this, &ActionListView::actionExecuted);
  connect(m_model, &ActionPanelModel::closeRequested, this, &ActionListView::closeRequested);
  connect(m_model, &ActionPanelModel::submenuActivated, this, [this](SubmenuAction *action) {
    auto *child = createSubmenuChild(action);
    if (child) emit pushViewRequested(child);
  });
}

ActionListView::~ActionListView() = default;

void ActionListView::adoptState(std::unique_ptr<ActionPanelState> state) {
  if (!state) {
    m_state = std::make_unique<ActionPanelState>();
  } else {
    m_state = std::move(state);
  }
  setId(m_state->id());
  m_state->finalize();

  m_model->reconcile(m_state.get());
  emit contentChanged();
}

QUrl ActionListView::componentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")); }

QVariantMap ActionListView::componentProps() {
  QVariantMap props;
  props[QStringLiteral("model")] = QVariant::fromValue(static_cast<QObject *>(m_model));
  return props;
}

AbstractAction *ActionListView::findBoundAction(const QKeyEvent *event) const {
  if (!m_state) return nullptr;

  for (const auto &section : m_state->sections()) {
    for (const auto &action : section->actions()) {
      if (action->isBoundTo(event)) { return action.get(); }
    }
  }
  return nullptr;
}

AbstractAction *ActionListView::primaryAction() const { return m_state ? m_state->primaryAction() : nullptr; }

std::shared_ptr<AbstractAction> ActionListView::retainAction(AbstractAction *action) const {
  if (!m_state) return nullptr;

  for (const auto &section : m_state->sections()) {
    for (const auto &a : section->actions()) {
      if (a.get() == action) return a;
    }
  }
  return nullptr;
}

bool ActionListView::hasActions() const { return m_state && m_state->actionCount() > 0; }

bool ActionListView::hasMultipleActions() const { return m_state && m_state->actionCount() > 1; }

void ActionListView::resetState() { m_model->setStateFrom(m_state.get()); }

ActionListView *ActionListView::createSubmenuChild(SubmenuAction *action) {
  auto state = action->createSubmenuState();
  if (!state) return nullptr;
  auto *child = new ActionListView(this);
  child->adoptState(std::move(state));
  return child;
}
