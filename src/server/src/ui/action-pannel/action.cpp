#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/action-list-view.hpp"
#include "ui/action-pannel/action-panel-state.hpp"

ActionPanelView *ListSubmenuAction::createView(QObject *parent) {
  auto state = buildState();
  if (!state) return nullptr;

  auto *view = new ActionListView(parent);
  view->adoptState(std::move(state));
  return view;
}
