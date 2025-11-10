#pragma once
#include "extend/image-model.hpp"
#include "proto/ui.pb.h"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <filesystem>
#include <qnamespace.h>
#include <qstringview.h>
#include <vector>
#include <QString>

enum class ObjectFit { Contain, Fill, Stretch };
enum ImageURLType { Invalid, Builtin, Favicon, System, Http, Https, Local, Emoji, DataURI };

static std::vector<std::pair<QString, ImageURLType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System},
    {"http", Http},       {"https", Http},       {"local", Local},
};

static std::vector<std::pair<QString, SemanticColor>> colorTints = {
    {"blue", SemanticColor::Blue},
    {"green", SemanticColor::Green},
    {"magenta", SemanticColor::Magenta},
    {"purple", SemanticColor::Purple},
    {"orange", SemanticColor::Orange},
    {"red", SemanticColor::Red},
    {"yellow", SemanticColor::Yellow},
    {"primary-text", SemanticColor::Foreground},
    {"secondary-text", SemanticColor::TextMuted}};

class ImageURL {
public:
  static Qt::AspectRatioMode fitToAspectRatio(ObjectFit fit);

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

  bool isValid() { return _isValid; }
  operator bool() { return isValid(); }

  QString toString() const { return url().toString(); }

  std::optional<QString> fallback() const { return _fallback; }
  ImageURL &withFallback(const ImageURL &fallback);

  bool isBuiltin() const { return type() == Builtin; }

  QUrl url() const;

  ImageURL &param(const QString &name, const QString &value);
  std::optional<QString> param(const QString &name) const;

  ImageURLType type() const;
  const QString &name() const;
  std::optional<SemanticColor> foregroundTint() const;
  std::optional<SemanticColor> backgroundTint() const;
  const std::optional<ColorLike> &fillColor() const;
  OmniPainter::ImageMaskType mask() const;
  std::optional<ObjectFit> fit() const { return m_fit; }
  QString cacheKey() const { return m_cacheKey.value_or(toString()); }
  void setCacheKey(const QString &key) { m_cacheKey = key; }
  bool cachable() const { return m_cache; }
  void setCachable(bool value) { m_cache = value; }

  void setFit(ObjectFit fit) { m_fit = fit; }
  void setType(ImageURLType type);
  void setName(const QString &name);
  void setSize(QSize size);

  ImageURL &setFill(const std::optional<ColorLike> &color);
  ImageURL &setMask(OmniPainter::ImageMaskType mask);
  ImageURL &setForegroundTint(SemanticColor tint);
  ImageURL &setBackgroundTint(SemanticColor tint);

  ImageURL();
  ImageURL(const QString &s) noexcept;
  ImageURL(const proto::ext::ui::Image &image);
  ImageURL(const ImageLikeModel &imageLike);
  ImageURL(const QUrl &url);

  bool operator==(const ImageURL &rhs) const;
  operator QString() const { return toString(); }

  static ImageURL builtin(const QString &name);
  static ImageURL favicon(const QString &domain);
  static ImageURL system(const QString &name);
  static ImageURL local(const QString &path);
  static ImageURL local(const std::filesystem::path &path);
  static ImageURL http(const QUrl &httpUrl);
  static ImageURL emoji(const QString &emoji);
  static ImageURL rawData(const QByteArray &data, const QString &mimeType);
  static ImageURL mimeType(const std::filesystem::path &path);

private:
  ImageURLType _type = ImageURLType::Invalid;
  bool _isValid = false;
  QString _name;
  bool m_cache = true;
  std::optional<SemanticColor> _bgTint;
  std::optional<SemanticColor> _fgTint;
  OmniPainter::ImageMaskType _mask = OmniPainter::ImageMaskType::NoMask;
  std::optional<QString> _fallback;
  std::optional<ColorLike> _fillColor = std::nullopt;
  std::optional<ObjectFit> m_fit;
  std::optional<QString> m_cacheKey;

  // url dependant custom params
  std::map<QString, QString> m_params;
};
