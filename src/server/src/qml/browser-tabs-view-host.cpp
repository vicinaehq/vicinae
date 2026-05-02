#include "browser-tabs-view-host.hpp"
#include "browser-tabs-model.hpp"
#include "service-registry.hpp"

void BrowserTabsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_section);

  setSearchPlaceholderText("Search, focus and close tabs");

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::tabsChanged, this, &BrowserTabsViewHost::reload);
}

void BrowserTabsViewHost::loadInitialData() { reload(); }

void BrowserTabsViewHost::reload() { m_section.setItems(context()->services->browserExtension()->tabs()); }
