#include "qml-provider-search-model.hpp"
#include "service-registry.hpp"

void QmlProviderSearchModel::setItems(std::vector<RootItemManager::ScoredItem> items) {
  m_items = std::move(items);

  std::vector<SectionInfo> sections;
  if (!m_items.empty())
    sections.push_back(
        {.name = QStringLiteral("Results (%1)").arg(m_items.size()),
         .count = static_cast<int>(m_items.size())});
  setSections(sections);
}

const RootItem *QmlProviderSearchModel::itemAt(int item) const {
  return m_items.at(item).item.get().get();
}

QString QmlProviderSearchModel::itemTitle(int, int i) const { return itemAt(i)->displayName(); }

QString QmlProviderSearchModel::itemSubtitle(int, int i) const { return itemAt(i)->subtitle(); }

QString QmlProviderSearchModel::itemIconSource(int, int i) const {
  return imageSourceFor(itemAt(i)->iconUrl());
}

std::unique_ptr<ActionPanelState> QmlProviderSearchModel::createActionPanel(int, int i) const {
  auto *item = itemAt(i);
  auto *manager = ctx()->services->rootItemManager();
  return item->newActionPanel(ctx(), manager->itemMetadata(item->uniqueId()));
}
