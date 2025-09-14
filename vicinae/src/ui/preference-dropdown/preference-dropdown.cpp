#include "preference-dropdown.hpp"
#include "ui/form/selector-input.hpp"
#include <qwidget.h>

void PreferenceDropdown::setOptions(const std::vector<Preference::DropdownData::Option> &opts) {
  std::vector<std::shared_ptr<AbstractItem>> items;

  for (const auto &opt : opts) {
    items.emplace_back(std::make_shared<PreferenceDropdownItem>(opt));
  }

  addSection("", items);
  updateModel();
}

PreferenceDropdown::PreferenceDropdown(QWidget *parent) : SelectorInput(parent) {}
