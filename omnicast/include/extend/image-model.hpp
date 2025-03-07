#pragma once
#include <qjsonobject.h>
#include <qstring.h>

struct ThemeIconModel {
  QString iconName;
  QString theme;
};

struct ImageFileModel {
  QString path;
};

struct ImageUrlModel {
  QString url;
};

using ImageLikeModel =
    std::variant<ImageUrlModel, ThemeIconModel, ImageFileModel>;

class ImageModelParser {
public:
  ImageModelParser();

  ImageLikeModel parse(const QJsonObject &root);
};
