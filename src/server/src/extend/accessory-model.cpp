#include "extend/accessory-model.hpp"

ListAccessory AccessoryModel::toAccessory() const {
  ListAccessory acc;

  acc.icon = icon;
  acc.tooltip = tooltip ? QString::fromStdString(*tooltip) : QString();

  if (auto tag = std::get_if<AccessoryModel::Tag>(&data)) {
    acc.color = tag->color;
    acc.fillBackground = true;
    acc.text = QString::fromStdString(tag->value);
  } else if (auto text = std::get_if<AccessoryModel::Text>(&data)) {
    acc.color = text->color;
    acc.text = QString::fromStdString(text->value);
  }

  return acc;
}
