#include "manage-fallback-view-host.hpp"
#include "manage-fallback-model.hpp"
#include "service-registry.hpp"

void ManageFallbackViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_enabledSection);
  model()->addSource(&m_availableSection);

  setSearchPlaceholderText("Search commands...");

  auto manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::fallbackEnabled, this, &ManageFallbackViewHost::reload);
  connect(manager, &RootItemManager::fallbackDisabled, this, &ManageFallbackViewHost::reload);
}

void ManageFallbackViewHost::loadInitialData() { reload(); }

void ManageFallbackViewHost::reload() {
  auto manager = context()->services->rootItemManager();

  auto fallbacks = manager->fallbackItems();
  m_enabledSection.setFallbackOrder(fallbacks);

  auto results = manager->search("");
  std::vector<RootItemPtr> enabled;
  std::vector<RootItemPtr> available;
  for (const auto &scored : results) {
    auto item = scored.item.get().get();
    if (!item->isSuitableForFallback()) continue;
    auto ptr = scored.item.get();
    if (std::ranges::find(fallbacks, ptr) != fallbacks.end())
      enabled.emplace_back(std::move(ptr));
    else
      available.emplace_back(std::move(ptr));
  }

  m_enabledSection.setItems(std::move(enabled));
  m_availableSection.setItems(std::move(available));
}
