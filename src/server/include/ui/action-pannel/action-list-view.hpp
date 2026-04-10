#pragma once
#include "ui/action-pannel/action-panel-view.hpp"
#include <memory>

class ActionPanelState;
class ActionPanelSectionState;

/**
 * Default action panel view: a fuzzy-searchable list of actions, optionally
 * grouped into sections. This is what every action panel rendered today
 * actually is.
 *
 * Internally wraps an `ActionPanelState` so that existing call sites can
 * adopt the view abstraction without rebuilding their action panels. The
 * legacy `setActions(unique_ptr<ActionPanelState>)` overload constructs an
 * `ActionListView` and calls `adoptState()` to forward the wrapped state.
 *
 * Step 1 note: `componentUrl`/`componentProps` are placeholders. The
 * controller still operates on `ActionPanelState` snapshots, so these methods
 * are not yet called by anything. They will be wired up in the controller
 * refactor that follows.
 */
class ActionListView : public ActionPanelView {
  Q_OBJECT

public:
  explicit ActionListView(QObject *parent = nullptr);
  ~ActionListView() override;

  /**
   * Replace the wrapped state. Used by the legacy compat shim to wrap an
   * externally-built `ActionPanelState` (including `ListActionPanelState` /
   * `FormActionPanelState` subclasses, whose preset is preserved through the
   * `unique_ptr` upcast).
   */
  void adoptState(std::unique_ptr<ActionPanelState> state);

  /**
   * Read-only access to the wrapped state.
   *
   * Transitional: exists so the legacy `NavigationController::setActions`
   * path can extract the state out of the view and forward it through the
   * existing snapshot machinery. Will be removed once the controller operates
   * on views directly.
   */
  const ActionPanelState *state() const { return m_state.get(); }

  /**
   * Take ownership of the wrapped state, leaving the view empty.
   *
   * Transitional: see `state()` above.
   */
  std::unique_ptr<ActionPanelState> takeState();

  // ActionPanelView overrides:
  QUrl componentUrl() const override;
  QVariantMap componentProps() const override;
  AbstractAction *findBoundAction(const QKeyEvent *event) const override;
  AbstractAction *primaryAction() const override;

private:
  std::unique_ptr<ActionPanelState> m_state;
};
