#include "theme-selector.hpp"

ThemeSelector::ThemeSelector() {
  auto &theme = ThemeService::instance();
  std::vector<std::shared_ptr<AbstractItem>> items;

  for (const auto &theme : theme.themes()) {
    items.emplace_back(std::make_shared<ThemeSelectorItem>(theme));
  }

  addSection("", items);
  updateModel();
}
