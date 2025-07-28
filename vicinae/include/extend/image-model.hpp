#pragma once
#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <qjsonobject.h>
#include <filesystem>
#include <qstring.h>
#include <variant>

struct ThemedIconSource {
  QString light;
  QString dark;
};

struct ExtensionImageModel {
  std::variant<QString, ThemedIconSource> source;
  std::optional<QString> fallback;
  std::optional<SemanticColor> tintColor;
  std::optional<OmniPainter::ImageMaskType> mask;
};

struct ExtensionFileIconModel {
  std::filesystem::path file;
};

using InvalidImageModel = std::monostate;

using ImageLikeModel =
    std::variant<InvalidImageModel, ExtensionImageModel, ExtensionFileIconModel, ThemedIconSource>;

class ImageModelParser {
public:
  ImageModelParser();

  ImageLikeModel parse(const QJsonObject &root);
};
