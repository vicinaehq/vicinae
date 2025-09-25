#include "app-selector.hpp"
#include "ui/form/selector-input.hpp"
#include <memory>
#include <qwidget.h>

AppSelector::AppSelector(QWidget *parent) : SelectorInput(parent) {}

void AppSelector::setApps(const std::vector<std::shared_ptr<AbstractApplication>> &apps) {
  std::vector<std::shared_ptr<AbstractItem>> items;

  for (const auto &app : apps) {
    items.emplace_back(std::make_shared<AppSelectorItem2>(app));
  }

  addSection("", items);
  updateModel();
}

AppSelectorItem2 const *AppSelector::value() const {
  return static_cast<AppSelectorItem2 const *>(SelectorInput::value());
}
