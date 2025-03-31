#include "extend/image-model.hpp"
#include <qjsonobject.h>

ImageModelParser::ImageModelParser() {}

ImageLikeModel ImageModelParser::parse(const QJsonObject &imageLike) {
  ImageLikeModel model;

  if (imageLike.contains("source")) {
    ExtensionImageModel model;
    auto source = imageLike.value("source").toString();

    model.source = imageLike.value("source").toString();

    // TODO: parse fallback, tint and mask

    return model;
  }

  if (imageLike.contains("fileIcon")) {
    ExtensionFileIconModel model;

    model.file = imageLike.value("fileIcon").toString().toStdString();

    return model;
  }

  return InvalidImageModel();
}
