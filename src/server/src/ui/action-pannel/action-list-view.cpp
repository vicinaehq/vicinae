#include "ui/action-pannel/action-list-view.hpp"
#include "action-panel-model.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include <QKeyEvent>

ActionListView::ActionListView(QObject *parent)
    : ActionPanelView(parent), m_state(std::make_unique<ActionPanelState>()) {}

ActionListView::~ActionListView() = default;

void ActionListView::adoptState(std::unique_ptr<ActionPanelState> state) {
  if (!state) {
    m_state = std::make_unique<ActionPanelState>();
  } else {
    m_state = std::move(state);
  }
  m_state->finalize();
  emit contentChanged();
}

QUrl ActionListView::componentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")); }

QVariantMap ActionListView::componentProps() {
  delete m_model;
  m_model = new ActionPanelModel(m_state.get(), this);

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

void ActionListView::resetState() {
  if (m_model) { m_model->setStateFrom(m_state.get()); }
}
