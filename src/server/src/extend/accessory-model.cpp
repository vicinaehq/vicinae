#include "extend/accessory-model.hpp"
#include "extend/color-model.hpp"

AccessoryModel AccessoryModel::fromGeneric(const glz::generic &value) {
  if (!value.is_object()) return {};
  const auto &obj = value.get_object();
  AccessoryModel model;

  if (obj.contains("icon")) { model.icon = ImageModelParser().parse(obj.at("icon")); }

  if (obj.contains("tooltip") && obj.at("tooltip").is_string()) {
    model.tooltip = QString::fromStdString(obj.at("tooltip").get_string());
  }

  if (obj.contains("tag")) {
    Tag tag;
    auto &tagValue = obj.at("tag");

    if (tagValue.is_string()) {
      tag.value = QString::fromStdString(tagValue.get_string());
    } else if (tagValue.is_object()) {
      const auto &tagObj = tagValue.get_object();
      if (tagObj.contains("value") && tagObj.at("value").is_string()) {
        tag.value = QString::fromStdString(tagObj.at("value").get_string());
      }
      if (tagObj.contains("color")) { tag.color = ColorLikeModelParser().parse(tagObj.at("color")); }
    }

    model.data = tag;
  }

  if (obj.contains("text")) {
    Text text;
    auto &textValue = obj.at("text");

    if (textValue.is_string()) {
      text.value = QString::fromStdString(textValue.get_string());
    } else if (textValue.is_object()) {
      const auto &textObj = textValue.get_object();
      if (textObj.contains("value") && textObj.at("value").is_string()) {
        text.value = QString::fromStdString(textObj.at("value").get_string());
      }
      if (textObj.contains("color")) { text.color = ColorLikeModelParser().parse(textObj.at("color")); }
    }

    model.data = text;
  }

  return model;
}

ListAccessory AccessoryModel::toAccessory() const {
  ListAccessory acc;

  acc.icon = icon;
  acc.tooltip = tooltip.value_or("");

  if (auto tag = std::get_if<AccessoryModel::Tag>(&data)) {
    acc.color = tag->color;
    acc.fillBackground = true;
    acc.text = tag->value;
  } else if (auto text = std::get_if<AccessoryModel::Text>(&data)) {
    acc.color = text->color;
    acc.text = text->value;
  }

  return acc;
}
