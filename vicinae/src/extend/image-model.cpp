#include "extend/image-model.hpp"
#include "extend/color-model.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <qjsonobject.h>

ImageModelParser::ImageModelParser() {}

ImageLikeModel ImageModelParser::parse(const QJsonValue &imageLike) {
  ImageLikeModel model;

  if (imageLike.isString()) { return ExtensionImageModel{.source = imageLike.toString()}; }
  if (!imageLike.isObject()) { return InvalidImageModel(); }

  auto obj = imageLike.toObject();

  if (obj.contains("source")) {
    ExtensionImageModel model;
    auto source = obj.value("source");

    if (source.isObject()) {
      auto obj = source.toObject();

      model.source = ThemedIconSource{
          .light = obj.value("light").toString(),
          .dark = obj.value("dark").toString(),
      };
    } else {
      model.source = obj.value("source").toString();
    }

    if (obj.contains("fallback")) { model.fallback = obj.value("fallback").toString(); }

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
