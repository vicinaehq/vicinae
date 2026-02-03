#include "extend/accessory-model.hpp"
#include "extend/color-model.hpp"

AccessoryModel AccessoryModel::fromJson(const QJsonValue &value) {
  if (!value.isObject()) return {};
  auto obj = value.toObject();
  AccessoryModel model;

  if (obj.contains("icon")) { model.icon = ImageModelParser().parse(obj.value("icon")); }
  if (obj.contains("tooltip")) { model.tooltip = obj.value("tooltip").toString(); }
  if (obj.contains("tag")) {
    Tag tag;
    auto tagValue = obj.value("tag");

    if (tagValue.isString()) tag.value = tagValue.toString();
    if (tagValue.isObject()) {
      auto tagObj = tagValue.toObject();
      tag.value = tagObj.value("value").toString();
      if (tagObj.contains("color")) { tag.color = ColorLikeModelParser().parse(tagObj.value("color")); }
    }

    model.data = tag;
  }
  if (obj.contains("text")) {
    Text text;
    auto textValue = obj.value("text");

    if (textValue.isString()) text.value = textValue.toString();
    if (textValue.isObject()) {
      auto textObj = textValue.toObject();
      text.value = textObj.value("value").toString();
      if (textObj.contains("color")) { text.color = ColorLikeModelParser().parse(textObj.value("color")); }
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
