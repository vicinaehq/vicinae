#pragma once
#include "builtins/browser/browser-tabs-model.hpp"
#include "ui/views/list-view-host.hpp"

class BrowserTabsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  BrowserTabsSection m_mediaSection{BrowserTabsSection::Kind::PlayingMedia};
  BrowserTabsSection m_tabsSection{BrowserTabsSection::Kind::Tabs};
};
