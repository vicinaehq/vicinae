#include "browser-tabs-view-host.hpp"
#include "browser-tabs-model.hpp"
#include "service-registry.hpp"

void BrowserTabsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_mediaSection);
  model()->addSource(&m_tabsSection);

  setSearchPlaceholderText(tr("Search, focus and close tabs"));

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::tabsChanged, this, &BrowserTabsViewHost::reload);
}

void BrowserTabsViewHost::loadInitialData() { reload(); }

void BrowserTabsViewHost::reload() {
  auto tabs = context()->services->browserExtension()->tabs();
  std::vector<BrowserTab> mediaTabs;
  std::vector<BrowserTab> regularTabs;

  mediaTabs.reserve(tabs.size());
  regularTabs.reserve(tabs.size());

  for (auto &tab : tabs) {
    if (tab.audible)
      mediaTabs.emplace_back(std::move(tab));
    else
      regularTabs.emplace_back(std::move(tab));
  }

  m_mediaSection.setItems(std::move(mediaTabs));
  m_tabsSection.setItems(std::move(regularTabs));
}
