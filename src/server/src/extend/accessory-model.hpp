#pragma once
#include "extend/image-model.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <glaze/json/generic.hpp>

class AccessoryModel {
public:
  struct Tag {
    std::optional<ColorLike> color;
    QString value;
  };

  struct Text {
    std::optional<ColorLike> color;
    QString value;
  };
  using Data = std::variant<Tag, Text>;

  ListAccessory toAccessory() const;
  static AccessoryModel fromGeneric(const glz::generic &value);

  Data data = Text{};
  std::optional<ImageLikeModel> icon;
  std::optional<QString> tooltip;
};
