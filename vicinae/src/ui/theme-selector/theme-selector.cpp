#include "theme-selector.hpp"

ThemeSelector::ThemeSelector() {
  auto &theme = ThemeService::instance();

  auto items = theme.themes() |
               std::views::transform([](auto &&theme) -> std::shared_ptr<SelectorInput::AbstractItem> {
                 return std::make_shared<ThemeSelectorItem>(theme);
               }) |
               std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}
