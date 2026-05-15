#include "extension-action-list-view.hpp"
#include "action-panel-model.hpp"
#include "ui/action-pannel/action.hpp"
#include <utility>

ExtensionActionListView::ExtensionActionListView(ExtensionActionPanelBuilder::NotifyFn notify,
                                                 const QString &onSearchTextChangeHandler,
                                                 ExtensionActionPanelBuilder::SubmenuCache *cache,
                                                 QObject *parent)
    : ActionListView(parent), m_notify(std::move(notify)),
      m_onSearchTextChangeHandler(onSearchTextChangeHandler), m_cache(cache) {}

QVariantMap ExtensionActionListView::componentProps() {
  auto props = ActionListView::componentProps();

  if (!m_onSearchTextChangeHandler.isEmpty() && model()) {
    connect(model(), &ActionPanelModel::filterChanged, this,
            [this](const QString &text) { m_notify(m_onSearchTextChangeHandler, {text}); });
  }

  return props;
}

ActionListView *ExtensionActionListView::createSubmenuChild(SubmenuAction *action) {
  auto state = action->createSubmenuState();
  if (!state) return nullptr;

  QString onSearchTextChange;
  QString const stableId = action->id();

  if (m_cache && !stableId.isEmpty()) {
    auto it = m_cache->find(stableId);
    if (it != m_cache->end()) { onSearchTextChange = it->second->onSearchTextChange; }
  }

  auto *child = new ExtensionActionListView(m_notify, onSearchTextChange, m_cache, this);
  child->adoptState(std::move(state));
  return child;
}
