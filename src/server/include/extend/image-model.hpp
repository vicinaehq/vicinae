#pragma once
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <filesystem>
#include <glaze/json/generic.hpp>
#include <qstring.h>
#include <variant>

struct ThemedIconSource {
  QString light;
  QString dark;
};

struct ExtensionImageModel {
  std::variant<QString, ThemedIconSource> source;
  std::optional<std::variant<QString, ThemedIconSource>> fallback;
  std::optional<ColorLike> tintColor;
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
  ImageLikeModel parse(const glz::generic &imageLike);
};
