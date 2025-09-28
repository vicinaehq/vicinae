#pragma once
#include "ui/views/list-view.hpp"

class BrowseAppsView : public SearchableListView {
private:
  QString sectionName() const override { return "Applications"; }
  QString initialSearchPlaceholderText() const override { return "Search apps..."; }
  Data initData() const override;
};
