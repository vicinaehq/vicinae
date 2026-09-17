#include "ui/action-panel/action.hpp"
#include "ui/action-panel/action-list-view.hpp"
#include "ui/action-panel/action-panel-state.hpp"

ActionPanelView *ListSubmenuAction::createView(ApplicationContext *ctx, QObject *parent) {
  auto state = buildState(ctx);
  if (!state) return nullptr;

  auto *view = new ActionListView(parent);
  view->adoptState(std::move(state));
  return view;
}
