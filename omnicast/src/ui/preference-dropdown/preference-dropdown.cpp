#include "preference-dropdown.hpp"
#include "ui/form/selector-input.hpp"
#include <qwidget.h>
#include <ranges>

void PreferenceDropdown::setOptions(const std::vector<Preference::DropdownData::Option> &opts) {
  auto transform = [&](auto &&option) -> std::shared_ptr<AbstractItem> {
    return std::make_shared<PreferenceDropdownItem>(option);
  };

  auto items = opts | std::views::transform(transform) | std::ranges::to<std::vector>();

  addSection("", items);
  updateModel();
}

PreferenceDropdown::PreferenceDropdown(QWidget *parent) : SelectorInput(parent) {}
