#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qjsonobject.h>
#include <filesystem>
#include <qstring.h>
#include <variant>

struct ThemeAwareIconSource {
  QString light;
  QString dark;
};

struct ExtensionImageModel {
  QString source;
  std::optional<QString> fallback;
  std::optional<SemanticColor> tintColor;
  std::optional<OmniPainter::ImageMaskType> mask;
};

struct ExtensionFileIconModel {
  std::filesystem::path file;
};

using InvalidImageModel = std::monostate;

using ImageLikeModel = std::variant<InvalidImageModel, ExtensionImageModel, ExtensionFileIconModel>;

class ImageModelParser {
public:
  ImageModelParser();

  ImageLikeModel parse(const QJsonObject &root);
};
