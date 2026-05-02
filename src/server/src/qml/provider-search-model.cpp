#include "provider-search-model.hpp"
#include "service-registry.hpp"

void ProviderSearchSection::setItems(std::vector<RootItemManager::ScoredItem> items) {
  m_items = std::move(items);
  notifyChanged();
}

const RootItem *ProviderSearchSection::itemAt(int i) const { return m_items.at(i).item.get().get(); }

QString ProviderSearchSection::itemTitle(int i) const { return itemAt(i)->title(); }

QString ProviderSearchSection::itemSubtitle(int i) const { return itemAt(i)->subtitle(); }

QString ProviderSearchSection::itemIconSource(int i) const { return imageSourceFor(itemAt(i)->iconUrl()); }

std::unique_ptr<ActionPanelState> ProviderSearchSection::actionPanel(int i) const {
  auto *item = itemAt(i);
  auto *manager = scope().services()->rootItemManager();
  return item->newActionPanel(scope().appContext(), manager->itemMetadata(item->uniqueId()));
}
