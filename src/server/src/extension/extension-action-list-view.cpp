#include "extension-action-list-view.hpp"
#include "action-panel-model.hpp"
#include "ui/action-pannel/action.hpp"
#include <utility>

ExtensionActionListView::ExtensionActionListView(ExtensionActionPanelBuilder::NotifyFn notify,
                                                 const QString &onSearchTextChangeHandler, QObject *parent)
    : ActionListView(parent), m_notify(std::move(notify)),
      m_onSearchTextChangeHandler(onSearchTextChangeHandler) {
  if (!m_onSearchTextChangeHandler.isEmpty()) {
    connect(model(), &ActionPanelModel::filterChanged, this,
            [this](const QString &text) { m_notify(m_onSearchTextChangeHandler, {text}); });
  }
}

ActionListView *ExtensionActionListView::createSubmenuChild(SubmenuAction *action) {
  auto state = action->createSubmenuState();
  if (!state) return nullptr;

  auto *child = new ExtensionActionListView(m_notify, action->onSearchTextChangeHandler(), this);
  child->adoptState(std::move(state));
  return child;
}
