#include "extend/color-model.hpp"
#include <qjsonobject.h>

ColorLikeModelParser::ColorLikeModelParser() {}

ColorLikeModel ColorLikeModelParser::parse(const QJsonObject &colorLike) {
  if (colorLike.contains("themeColor")) {
    return colorLike.value("themeColor").toString();
  }

  if (colorLike.contains("colorString")) {
    return colorLike.value("colorString").toString();
  }

  return "primary-text";
}
