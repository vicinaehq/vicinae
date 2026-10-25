#pragma once
#include <optional>
#include <string>
#include <vector>

#include "builtins/browser/browser-tabs-model.hpp"
#include "ui/quick/completion-model.hpp"
#include "ui/views/list-view-host.hpp"

class BrowserTabsViewHost : public ListViewHost {
  Q_OBJECT
  Q_PROPERTY(CompletionModel *categoryFilterModel READ categoryFilterModel CONSTANT)
  Q_PROPERTY(int currentCategoryFilter READ currentCategoryFilter NOTIFY currentCategoryFilterChanged)

signals:
  void currentCategoryFilterChanged();

public:
  QUrl qmlSearchAccessoryUrl() const override { return qml::componentUrl(u"CategoryFilterAccessory"); }
  CompletionModel *categoryFilterModel() { return &m_categoryFilterModel; }
  int currentCategoryFilter() const { return m_currentCategoryFilter; }
  Q_INVOKABLE void setCategoryFilter(int index);
  void initialize() override;
  void loadInitialData() override;

private:
  void refreshBrowserFilters();
  void reload();

  BrowserTabsSection m_pinnedSection{BrowserTabsSection::Kind::Pinned};
  BrowserTabsSection m_mediaSection{BrowserTabsSection::Kind::PlayingMedia};
  BrowserTabsSection m_tabsSection{BrowserTabsSection::Kind::Tabs};
  CompletionModel m_categoryFilterModel{this};
  std::vector<std::string> m_browserIds;
  std::optional<std::string> m_selectedBrowserId;
  int m_currentCategoryFilter = 0;
};
