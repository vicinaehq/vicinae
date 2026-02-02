#include "extend/color-model.hpp"
#include "ui/image/url.hpp"
#include "theme.hpp"
#include <qjsonobject.h>

ColorLikeModelParser::ColorLikeModelParser() {}

ColorLike ColorLikeModelParser::parse(const QJsonValue &colorLike) {
  auto colorLikeObj = colorLike.toObject();

  if (colorLikeObj.isEmpty()) {
    qWarning() << "ColorLikeModelParser: Input JSON object is empty. colorLike =" << colorLike;
    return QColor();
  }

  if (colorLikeObj.contains("raw")) {
    auto raw = colorLikeObj.value("raw").toString();
    if (auto tint = ImageURL::tintForName(raw); tint != SemanticColor::InvalidTint) { return tint; }
    return raw;
  }

  if (colorLikeObj.contains("dynamic")) {
    auto dynamic = colorLikeObj.value("dynamic").toObject();
    return DynamicColor{.light = dynamic.value("light").toString(),
                        .dark = dynamic.value("dark").toString(),
                        .adjustContrast = dynamic.value("adjustContrast").toBool(true)};
  }

  qWarning() << "ColorLikeModelParser: Invalid colorLike input. colorLike =" << colorLike;
  return QColor();
}
