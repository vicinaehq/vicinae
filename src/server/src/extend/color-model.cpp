#include "extend/color-model.hpp"
#include "ui/image/url.hpp"

ColorLike ColorLikeModelParser::parse(const glz::generic &colorLike) {
  if (!colorLike.is_object()) return QColor();

  const auto &obj = colorLike.get_object();

  if (obj.contains("raw")) {
    auto &raw = obj.at("raw");
    if (!raw.is_string()) return QColor();
    auto rawStr = QString::fromStdString(raw.get_string());
    if (auto tint = ImageURL::tintForName(rawStr); tint != SemanticColor::InvalidTint) { return tint; }
    return rawStr;
  }

  if (obj.contains("dynamic")) {
    auto &dynamic = obj.at("dynamic");
    if (!dynamic.is_object()) return QColor();
    const auto &dynObj = dynamic.get_object();

    auto light = dynObj.contains("light") && dynObj.at("light").is_string()
                     ? QString::fromStdString(dynObj.at("light").get_string())
                     : QString();
    auto dark = dynObj.contains("dark") && dynObj.at("dark").is_string()
                    ? QString::fromStdString(dynObj.at("dark").get_string())
                    : QString();
    bool adjustContrast = true;
    if (dynObj.contains("adjustContrast") && dynObj.at("adjustContrast").is_boolean()) {
      adjustContrast = dynObj.at("adjustContrast").get_boolean();
    }

    return DynamicColor{.light = light, .dark = dark, .adjustContrast = adjustContrast};
  }

  return QColor();
}
