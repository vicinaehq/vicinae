#include <ranges>
#include "service-registry.hpp"
#include "font-selector.hpp"

FontSelector::FontSelector() {
  auto fonts = ServiceRegistry::instance()->fontService();
  auto items = fonts->families() |
               std::views::transform([](auto &&font) -> std::shared_ptr<SelectorInput::AbstractItem> {
                 return std::make_shared<FontSelectorItem>(font);
               }) |
               std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}
