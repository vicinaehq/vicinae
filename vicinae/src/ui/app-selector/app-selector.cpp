#include "app-selector.hpp"
#include "ui/form/selector-input.hpp"
#include <qwidget.h>

AppSelector::AppSelector(QWidget *parent) : SelectorInput(parent) {}

void AppSelector::setApps(const std::vector<std::shared_ptr<Application>> &apps) {
  auto transform = [&](auto &&option) -> std::shared_ptr<AbstractItem> {
    return std::make_shared<AppSelectorItem2>(option);
  };

  auto items = apps | std::views::transform(transform) | std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}

AppSelectorItem2 const *AppSelector::value() const {
  return static_cast<AppSelectorItem2 const *>(SelectorInput::value());
}
