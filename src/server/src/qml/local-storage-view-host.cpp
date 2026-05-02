#include "local-storage-view-host.hpp"
#include "service-registry.hpp"

void LocalStorageViewHost::initialize() {
  BaseView::initialize();
  initModel();
  model()->addSource(&m_section);
  setSearchPlaceholderText("Search namespaces...");
}

void LocalStorageViewHost::loadInitialData() {
  m_section.setItems(context()->services->localStorage()->namespaces());
}

LocalStorageItemViewHost::LocalStorageItemViewHost(const QString &ns, std::vector<QString> keys)
    : m_ns(ns), m_keys(std::move(keys)) {}

void LocalStorageItemViewHost::initialize() {
  BaseView::initialize();
  initModel();
  m_section.setNamespace(m_ns);
  model()->addSource(&m_section);
  setSearchPlaceholderText("Search items...");
}

void LocalStorageItemViewHost::loadInitialData() { m_section.setItems(std::move(m_keys)); }
