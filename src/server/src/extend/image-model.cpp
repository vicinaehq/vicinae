#include "extend/image-model.hpp"
#include "extend/color-model.hpp"

static std::variant<QString, ThemedIconSource> parseSource(const glz::generic &sourceValue) {
  if (sourceValue.is_string()) { return QString::fromStdString(sourceValue.get_string()); }
  if (!sourceValue.is_object()) { return QString(); }

  const auto &sourceObj = sourceValue.get_object();

  if (sourceObj.contains("raw")) {
    auto &raw = sourceObj.at("raw");
    return raw.is_string() ? QString::fromStdString(raw.get_string()) : QString();
  }

  if (sourceObj.contains("themed")) {
    auto &themed = sourceObj.at("themed");
    if (!themed.is_object()) return QString();
    const auto &themedObj = themed.get_object();

    return ThemedIconSource{
        .light = themedObj.contains("light") && themedObj.at("light").is_string()
                     ? QString::fromStdString(themedObj.at("light").get_string())
                     : QString(),
        .dark = themedObj.contains("dark") && themedObj.at("dark").is_string()
                    ? QString::fromStdString(themedObj.at("dark").get_string())
                    : QString(),
    };
  }

  return QString();
}

ImageLikeModel ImageModelParser::parse(const glz::generic &imageLike) {
  if (imageLike.is_string()) {
    return ExtensionImageModel{.source = QString::fromStdString(imageLike.get_string())};
  }
  if (!imageLike.is_object()) { return InvalidImageModel(); }

  const auto &obj = imageLike.get_object();

  if (obj.contains("source")) {
    ExtensionImageModel model;
    model.source = parseSource(obj.at("source"));

    if (obj.contains("fallback")) { model.fallback = parseSource(obj.at("fallback")); }

    if (obj.contains("tintColor")) { model.tintColor = ColorLikeModelParser().parse(obj.at("tintColor")); }

    if (obj.contains("mask") && obj.at("mask").is_string()) {
      model.mask = OmniPainter::maskForName(QString::fromStdString(obj.at("mask").get_string()));
    }

    return model;
  }

  if (obj.contains("fileIcon")) {
    auto &fileIcon = obj.at("fileIcon");
    if (fileIcon.is_string()) { return ExtensionFileIconModel{.file = fileIcon.get_string()}; }
  }

  return InvalidImageModel();
}
