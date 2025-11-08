#include "extend/image-model.hpp"
#include "extend/color-model.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qjsonobject.h>

ImageModelParser::ImageModelParser() {}

ImageLikeModel ImageModelParser::parse(const QJsonValue &imageLike) {
  if (imageLike.isString()) { return ExtensionImageModel{.source = imageLike.toString()}; }
  if (!imageLike.isObject()) { return InvalidImageModel(); }

  auto obj = imageLike.toObject();

  // Parses Image.Source / Image.Fallback
  auto parseSource = [](const QJsonValue &sourceValue) -> std::variant<QString, ThemedIconSource> {
    if (!sourceValue.isObject()) { return sourceValue.toString(); }
    auto sourceObj = sourceValue.toObject();

    if (sourceObj.contains("raw")) { return sourceObj.value("raw").toString(); }

    if (sourceObj.contains("themed")) {
      auto themedObj = sourceObj.value("themed").toObject();
      return ThemedIconSource{
          .light = themedObj.value("light").toString(),
          .dark = themedObj.value("dark").toString(),
      };
    }

    qWarning() << "ImageModelParser::parse: Invalid source value. sourceValue =" << sourceValue;
    return QString();
  };

  if (obj.contains("source")) {
    ExtensionImageModel model;
    model.source = parseSource(obj.value("source"));

    if (obj.contains("fallback")) { model.fallback = parseSource(obj.value("fallback")); }

    if (obj.contains("tintColor")) { model.tintColor = ColorLikeModelParser().parse(obj.value("tintColor")); }

    if (obj.contains("mask")) { model.mask = OmniPainter::maskForName(obj.value("mask").toString()); }

    return model;
  }

  if (obj.contains("fileIcon")) {
    ExtensionFileIconModel model;
    model.file = obj.value("fileIcon").toString().toStdString();

    return model;
  }

  return InvalidImageModel();
}
