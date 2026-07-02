#pragma once
#include "builtin_icon.hpp"
#include "extend/image-model.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <QUrl>
#include <cstdint>
#include <filesystem>
#include <qstringview.h>
#include <vector>
#include <QString>

enum class ObjectFit : std::uint8_t { Contain, Fill, Stretch };
enum ImageURLType : std::uint8_t {
  Invalid,
  Builtin,
  Favicon,
  System,
  Http,
  Https,
  Local,
  Emoji,
  Symbol,
  DataURI,
  MacBundle,
  FileIcon,
  FontPreview
};

static std::vector<std::pair<QString, ImageURLType>> iconTypes = {
    {"favicon", Favicon},
    {"omnicast", Builtin},
    {"builtin", Builtin},
    {"system", System},
    {"http", Http},
    {"https", Http},
    {"local", Local},
    {"bundle", MacBundle},
    {"file-icon", FileIcon},
    {"emoji", Emoji},
    {"symbol", Symbol},
    {"datauri", DataURI},
    {"font-preview", FontPreview},
};

static std::vector<std::pair<QString, SemanticColor>> colorTints = {
    {"blue", SemanticColor::Blue},
    {"green", SemanticColor::Green},
    {"magenta", SemanticColor::Magenta},
    {"purple", SemanticColor::Purple},
    {"orange", SemanticColor::Orange},
    {"red", SemanticColor::Red},
    {"yellow", SemanticColor::Yellow},
    {"cyan", SemanticColor::Cyan},
    {"accent", SemanticColor::Accent},
    {"primary-text", SemanticColor::Foreground},
    {"secondary-text", SemanticColor::TextMuted}};

class ImageURL {
public:
  ImageURL &circle() {
    setMask(OmniPainter::CircleMask);
    return *this;
  }

  static SemanticColor tintForName(const QString &name) {
    for (const auto &[n, t] : colorTints) {
      if (name == n) return t;
    }

    return SemanticColor::InvalidTint;
  }

  static QString nameForTint(SemanticColor type) {
    for (const auto &[n, t] : colorTints) {
      if (t == type) return n;
    }

    return {};
  }

  static ImageURLType typeForName(const QString &name) {
    for (const auto &[n, t] : iconTypes) {
      if (name == n) return t;
    }

    return ImageURLType::Invalid;
  }

  static QString nameForType(ImageURLType type) {
    for (const auto &[n, t] : iconTypes) {
      if (t == type) return n;
    }

    return {};
  }

  bool isValid() const { return _isValid; }
  operator bool() const { return isValid(); }

  QString toString() const { return url().toString(); }

  std::optional<QString> fallback() const { return _fallback; }
  ImageURL &withFallback(const ImageURL &fallback);

  bool isBuiltin() const { return type() == Builtin; }

  QUrl url() const;

  ImageURLType type() const;
  const QString &name() const;
  std::optional<ColorLike> backgroundTint() const;
  const std::optional<ColorLike> &fillColor() const;
  OmniPainter::ImageMaskType mask() const;

  void setType(ImageURLType type);
  void setName(const QString &name);

  ImageURL &setFill(const std::optional<ColorLike> &color);
  ImageURL &setMask(OmniPainter::ImageMaskType mask);
  ImageURL &setBackgroundTint(const ColorLike &tint);

  // Returns a copy with colors resolved against the current theme (SemanticColor/DynamicColor
  // become QColor; Builtin without fill gets Foreground) and local paths substituted with their
  // @light/@dark sibling when present on disk.
  ImageURL resolved() const;

  ImageURL();
  ImageURL(const QString &s) noexcept;
  ImageURL(BuiltinIcon icon);
  ImageURL(const ImageLikeModel &imageLike);
  ImageURL(const QUrl &url);

  operator QString() const { return toString(); }

  static ImageURL builtin(const QString &name);
  static ImageURL builtin(BuiltinIcon icon);
  static ImageURL favicon(const QString &domain);
  static ImageURL system(const QString &name);
  static ImageURL local(const QString &path);
  static ImageURL local(const std::filesystem::path &path);
  static ImageURL macBundle(const std::filesystem::path &bundlePath);
  static ImageURL http(const QUrl &httpUrl);
  static ImageURL emoji(const QString &emoji);
  static ImageURL symbol(const QString &symbol);
  static ImageURL fontPreview(const QString &family, const QString &glyph);
  static ImageURL rawData(const QByteArray &data, const QString &mimeType);
  static ImageURL fileIcon(const std::filesystem::path &path);

private:
  ImageURLType _type = ImageURLType::Invalid;
  bool _isValid = false;
  QString _name;
  std::optional<ColorLike> _bgTint;
  OmniPainter::ImageMaskType _mask = OmniPainter::ImageMaskType::NoMask;
  std::optional<QString> _fallback;
  std::optional<ColorLike> _fillColor = std::nullopt;
};
