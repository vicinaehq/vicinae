#pragma once
#include "builtin_icon.hpp"
#include "navigation-controller.hpp"
#include "ui/views/typed-list-view.hpp"

using SearchIconDataType = std::pair<BuiltinIcon, const char *>;

class SearchBuiltinIconView : public FilteredTypedListView<SearchIconDataType> {
  FilteredItemData mapFilteredData(const SearchIconDataType &item) const override {
    const auto &[icon, name] = item;
    return {.id = name, .title = name, .icon = icon};
  }

  std::unique_ptr<ActionPanelState> createActionPanel(const SearchIconDataType &type) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    return panel;
  }

  DataSet initializeDataSet() override {
    auto &mapping = BuiltinIconService::mapping();
    std::vector<SearchIconDataType> icons{mapping.begin(), mapping.end()};
    return {Section("Icons ({count})", icons)};
  }
};
