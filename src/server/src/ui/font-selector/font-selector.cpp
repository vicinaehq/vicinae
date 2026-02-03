#include "service-registry.hpp"
#include "font-selector.hpp"

FontSelector::FontSelector() {
  auto fonts = ServiceRegistry::instance()->fontService();
  std::vector<std::shared_ptr<AbstractItem>> items;

  for (const auto &family : fonts->families()) {
    items.emplace_back(std::make_shared<FontSelectorItem>(family));
  }

  addSection("", items);
  updateModel();
}
