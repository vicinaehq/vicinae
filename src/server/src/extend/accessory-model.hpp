#pragma once
#include "extend/image-model.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <optional>
#include <string>
#include <variant>

class AccessoryModel {
public:
  struct Tag {
    std::optional<ColorLike> color;
    std::string value;
  };

  struct Text {
    std::optional<ColorLike> color;
    std::string value;
  };
  using Data = std::variant<Tag, Text>;

  ListAccessory toAccessory() const;

  Data data = Text{};
  std::optional<ImageLikeModel> icon;
  std::optional<std::string> tooltip;
};
