#pragma once
#include "extend/image-model.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include <qjsonvalue.h>

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
  static AccessoryModel fromJson(const QJsonValue &value);

  Data data;
  std::optional<ImageLikeModel> icon;
  std::optional<QString> tooltip;
};
