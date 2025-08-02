#include "extend/color-model.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qjsonobject.h>

ColorLikeModelParser::ColorLikeModelParser() {}

ColorLike ColorLikeModelParser::parse(const QJsonValue &colorLike) {
  if (colorLike.isString()) {
    if (auto tint = OmniIconUrl::tintForName(colorLike.toString()); tint != SemanticColor::InvalidTint) {
      return tint;
    }

    return QColor(colorLike.toString());
  }

  return QColor();
}
