#include "extend/color-model.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include <qjsonobject.h>

ColorLikeModelParser::ColorLikeModelParser() {}

ColorLike ColorLikeModelParser::parse(const QJsonValue &colorLike) {
  if (colorLike.isString()) {
    if (auto tint = ImageURL::tintForName(colorLike.toString()); tint != SemanticColor::InvalidTint) {
      return tint;
    }

    return QColor(colorLike.toString());
  }

  if (colorLike.isObject()) {
    auto obj = colorLike.toObject();

    return DynamicColor{.light = obj.value("light").toString(),
                        .dark = obj.value("dark").toString(),
                        .adjustContrast = obj.value("adjustContrast").toBool(true)};
  }

  return QColor();
}
