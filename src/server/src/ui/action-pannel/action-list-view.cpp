#include "ui/action-pannel/action-list-view.hpp"
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
  emit contentChanged();
}

std::unique_ptr<ActionPanelState> ActionListView::takeState() {
  auto out = std::move(m_state);
  m_state = std::make_unique<ActionPanelState>();
  return out;
}

QUrl ActionListView::componentUrl() const { return QUrl(QStringLiteral("qrc:/Vicinae/ActionListPanel.qml")); }

QVariantMap ActionListView::componentProps() const {
  // Step 1: the controller does not yet push views onto the QML stack — it
  // still constructs `ActionPanelModel`s on demand from snapshots. This will
  // be filled in by the controller refactor that follows.
  return {};
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
