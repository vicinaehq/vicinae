#include "provider-search-view-host.hpp"
#include "provider-search-model.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

ProviderSearchViewHost::ProviderSearchViewHost(const RootProvider &provider)
    : m_providerId(provider.uniqueId()), m_displayName(provider.displayName()), m_icon(provider.icon()) {}

void ProviderSearchViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText(QString("Search %1").arg(m_displayName));
  setNavigationTitle(m_displayName);
  setNavigationIcon(m_icon);

  auto *manager = context()->services->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, [this]() { refresh(searchText()); });
}

void ProviderSearchViewHost::loadInitialData() { refresh({}); }

void ProviderSearchViewHost::textChanged(const QString &text) { refresh(text); }

void ProviderSearchViewHost::refresh(const QString &text) {
  auto *manager = context()->services->rootItemManager();
  auto results = manager->search(text, {.providerId = m_providerId.toStdString()});
  m_section.setItems(std::move(results));
}
