#pragma once
#include "browser-tabs-model.hpp"
#include "list-view-host.hpp"

class BrowserTabsViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;

private:
  void reload();

  BrowserTabsSection m_section;
};
