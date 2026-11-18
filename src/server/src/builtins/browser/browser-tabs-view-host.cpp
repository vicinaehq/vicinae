#include <unordered_map>

#include "builtins/browser/browser-tabs-view-host.hpp"
#include "builtins/browser/browser-tabs-model.hpp"
#include "service-registry.hpp"

void BrowserTabsViewHost::initialize() {
  BaseView::initialize();
  initModel();

  model()->addSource(&m_pinnedSection);
  model()->addSource(&m_mediaSection);
  model()->addSource(&m_tabsSection);

  setSearchPlaceholderText(tr("Search, focus and close tabs"));

  auto browser = context()->services->browserExtension();
  connect(browser, &BrowserExtensionService::browsersChanged, this,
          &BrowserTabsViewHost::refreshBrowserFilters);
  connect(browser, &BrowserExtensionService::tabsChanged, this, &BrowserTabsViewHost::reload);
  refreshBrowserFilters();
}

void BrowserTabsViewHost::loadInitialData() { reload(); }

void BrowserTabsViewHost::setCategoryFilter(int index) {
  if (index < 0 || std::cmp_greater(index, m_browserIds.size()) || index == m_currentCategoryFilter) return;
  m_currentCategoryFilter = index;
  m_selectedBrowserId = index == 0 ? std::nullopt : std::optional(m_browserIds[index - 1]);
  emit currentCategoryFilterChanged();
  reload();
}

void BrowserTabsViewHost::refreshBrowserFilters() {
  const auto browsers = context()->services->browserExtension()->browsers();
  std::unordered_map<std::string, int> labelCounts;
  labelCounts.reserve(browsers.size());
  for (const auto &browser : browsers)
    ++labelCounts[browser.name];

  QStringList options{tr("All browsers")};
  options.reserve(browsers.size() + 1);
  std::vector<std::string> browserIds;
  browserIds.reserve(browsers.size());
  int selectedIndex = 0;
  for (const auto &browser : browsers) {
    QString label = browser.name.empty() ? tr("Browser") : QString::fromStdString(browser.name);
    if (labelCounts[browser.name] > 1)
      label += QStringLiteral(" (%1)").arg(QString::fromStdString(browser.id.substr(0, 8)));
    options << label;
    browserIds.emplace_back(browser.id);
    if (m_selectedBrowserId && *m_selectedBrowserId == browser.id)
      selectedIndex = static_cast<int>(browserIds.size());
  }

  m_browserIds = std::move(browserIds);
  if (m_selectedBrowserId && selectedIndex == 0) {
    m_selectedBrowserId.reset();
    reload();
  }
  m_currentCategoryFilter = selectedIndex;
  m_categoryFilterModel.setStringOptions(options);
  emit currentCategoryFilterChanged();
}

void BrowserTabsViewHost::reload() {
  std::optional<std::string_view> browserId;
  if (m_selectedBrowserId) browserId = *m_selectedBrowserId;
  auto tabs = context()->services->browserExtension()->tabs(browserId);
  std::vector<BrowserTab> pinnedTabs;
  std::vector<BrowserTab> mediaTabs;
  std::vector<BrowserTab> regularTabs;

  pinnedTabs.reserve(tabs.size());
  mediaTabs.reserve(tabs.size());
  regularTabs.reserve(tabs.size());

  for (auto &tab : tabs) {
    if (tab.pinned)
      pinnedTabs.emplace_back(std::move(tab));
    else if (tab.audible)
      mediaTabs.emplace_back(std::move(tab));
    else
      regularTabs.emplace_back(std::move(tab));
  }

  m_pinnedSection.setItems(std::move(pinnedTabs));
  m_mediaSection.setItems(std::move(mediaTabs));
  m_tabsSection.setItems(std::move(regularTabs));
}
