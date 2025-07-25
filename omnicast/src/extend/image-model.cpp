#include "extend/image-model.hpp"
#include "omni-icon.hpp"
#include "ui/omni-painter.hpp"
#include <qjsonobject.h>

ImageModelParser::ImageModelParser() {}

ImageLikeModel ImageModelParser::parse(const QJsonObject &imageLike) {
  ImageLikeModel model;

  if (imageLike.contains("source")) {
    ExtensionImageModel model;
    auto source = imageLike.value("source");

    if (source.isObject()) {
      auto obj = source.toObject();

      model.source = ThemedIconSource{
          .light = obj.value("light").toString(),
          .dark = obj.value("dark").toString(),
      };
    } else {
      model.source = imageLike.value("source").toString();
    }

    if (imageLike.contains("fallback")) { model.fallback = imageLike.value("fallback").toString(); }

    if (imageLike.contains("tintColor")) {
      model.tintColor = OmniIconUrl::tintForName(imageLike.value("tintColor").toString());
    }

    if (imageLike.contains("mask")) {
      model.mask = OmniPainter::maskForName(imageLike.value("mask").toString());
    }

    return model;
  }

  if (imageLike.contains("fileIcon")) {
    ExtensionFileIconModel model;

    model.file = imageLike.value("fileIcon").toString().toStdString();

    return model;
  }

  return InvalidImageModel();
}
