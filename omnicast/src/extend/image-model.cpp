#include "extend/image-model.hpp"
#include <qjsonobject.h>

ImageModelParser::ImageModelParser() {}

ImageLikeModel ImageModelParser::parse(const QJsonObject &imageLike) {
  ImageLikeModel model;

  if (imageLike.contains("url")) {
    ImageUrlModel model;

    model.url = imageLike.value("url").toString();

    return model;
  }

  if (imageLike.contains("path")) {
    ImageFileModel model;

    model.path = imageLike.value("path").toString();

    return model;
  }

  if (imageLike.contains("iconName")) {
    ThemeIconModel model;

    model.iconName = imageLike.value("iconName").toString();
    model.theme = imageLike.value("theme").toString();

    return model;
  }

  return ThemeIconModel{.iconName = "application-x-executable"};
}
