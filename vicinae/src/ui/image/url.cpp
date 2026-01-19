#include "builtin_icon.hpp"
#include "emoji/emoji.hpp"
#include "extend/image-model.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "theme/theme-file.hpp"
#include <qdir.h>
#include <qmimedatabase.h>
#include <qstringview.h>
#include <QIcon>
#include <qurlquery.h>
#include "url.hpp"

namespace fs = std::filesystem;

Qt::AspectRatioMode ImageURL::fitToAspectRatio(ObjectFit fit) {
  switch (fit) {
  case ObjectFit::Fill:
    return Qt::KeepAspectRatioByExpanding;
  case ObjectFit::Contain:
    return Qt::KeepAspectRatio;
  case ObjectFit::Stretch:
    return Qt::IgnoreAspectRatio;
  }
  return Qt::IgnoreAspectRatio;
}

ImageURL &ImageURL::setFill(const std::optional<ColorLike> &color) {
  _fillColor = color;
  return *this;
}

ImageURL &ImageURL::setMask(OmniPainter::ImageMaskType mask) {
  _mask = mask;
  return *this;
}

ImageURL &ImageURL::setForegroundTint(SemanticColor tint) {
  _fgTint = tint;
  return *this;
}
ImageURL &ImageURL::setBackgroundTint(SemanticColor tint) {
  _bgTint = tint;
  return *this;
}

ImageURLType ImageURL::type() const { return _type; }
const QString &ImageURL::name() const { return _name; }
std::optional<SemanticColor> ImageURL::foregroundTint() const { return _fgTint; }
std::optional<SemanticColor> ImageURL::backgroundTint() const { return _bgTint; }
const std::optional<ColorLike> &ImageURL::fillColor() const { return _fillColor; }
OmniPainter::ImageMaskType ImageURL::mask() const { return _mask; }

ImageURL &ImageURL::withFallback(const ImageURL &fallback) {
  _fallback = fallback.toString();
  return *this;
}

QUrl ImageURL::url() const {
  QUrl url;

  url.setScheme("icon");
  url.setHost(nameForType(_type));
  url.setPath("/" + _name);

  QUrlQuery query;

  if (_fallback) query.addQueryItem("fallback", *_fallback);
  if (_bgTint) query.addQueryItem("bg_tint", nameForTint(*_bgTint));
  if (_fillColor) { query.addQueryItem("fill", OmniPainter::serializeColor(_fillColor.value())); }

  for (const auto &[k, v] : m_params) {
    query.addQueryItem(k, v);
  }

  url.setQuery(query);

  return url;
}

ImageURL &ImageURL::param(const QString &name, const QString &value) {
  m_params[name] = value;
  return *this;
}

std::optional<QString> ImageURL::param(const QString &name) const {
  if (auto it = m_params.find(name); it != m_params.end()) return it->second;

  return std::nullopt;
}

void ImageURL::setType(ImageURLType type) { _type = type; }
void ImageURL::setName(const QString &name) { _name = name; }

bool ImageURL::operator==(const ImageURL &rhs) const { return toString() == rhs.toString(); }

ImageURL::ImageURL(const QString &s) noexcept { *this = std::move(QUrl(s)); }

ImageURL::ImageURL() {}

ImageURL::ImageURL(const QUrl &url) : _mask(OmniPainter::NoMask) {
  if (url.scheme() != "icon") { return; }

  _type = typeForName(url.host());

  if (_type == Invalid) { return; }

  _name = url.path().sliced(1);

  auto query = QUrlQuery(url.query());

  if (auto fgTint = query.queryItemValue("fg_tint"); !fgTint.isEmpty()) { _fgTint = tintForName(fgTint); }
  if (auto bgTint = query.queryItemValue("bg_tint"); !bgTint.isEmpty()) { _bgTint = tintForName(bgTint); }
  if (auto fill = query.queryItemValue("fill"); !fill.isEmpty()) { _fillColor = tintForName(fill); }
  if (auto fallback = query.queryItemValue("fallback"); !fallback.isEmpty()) { _fallback = fallback; }
  if (auto mask = query.queryItemValue("mask"); !mask.isEmpty()) {
    if (mask == "circle")
      _mask = OmniPainter::ImageMaskType::CircleMask;
    else if (mask == "roundedRectangle")
      _mask = OmniPainter::ImageMaskType::RoundedRectangleMask;
  }

  for (const auto &[k, v] : query.queryItems()) {
    m_params[k] = v;
  }

  _isValid = true;
}

ImageURL::ImageURL(const ImageLikeModel &imageLike) : _mask(OmniPainter::NoMask) {
  if (auto image = std::get_if<ExtensionImageModel>(&imageLike)) {
    struct {
      QString operator()(const ThemedIconSource &icon) {
        if (ThemeService::instance().theme().isLight()) { return icon.light; }
        return icon.dark;
      }
      QString operator()(const QString &icon) { return icon; }
    } visitor;

    auto source = std::visit(visitor, image->source);

    QUrl url(source);

    if (auto fallback = image->fallback) {
      withFallback(ImageLikeModel(ExtensionImageModel{.source = *fallback}));
    }

    if (auto tintColor = image->tintColor) { setFill(*tintColor); }
    if (auto mask = image->mask) { setMask(*mask); }

    if (url.isValid()) {
      if (url.scheme() == "file") {
        setType(ImageURLType::Local);
        setName(url.host() + url.path());
        return;
      }

      if (url.scheme() == "data") {
        setType(ImageURLType::DataURI);
        setName(source);
        return;
      }

      if (url.scheme() == "https" || url.scheme() == "http") {
        setType(ImageURLType::Http);
        setName(url.toString());
        return;
      }
    }

    if (emoji::isUtf8EncodedEmoji(source.toStdString())) {
      setType(ImageURLType::Emoji);
      setName(source);
      return;
    }

    if (QFile(":icons/" + source + ".svg").exists()) {
      setType(ImageURLType::Builtin);
      setFill(image->tintColor.value_or(SemanticColor::Foreground));
      setName(source);
      return;
    }

    if (QFile(source).exists()) {
      setType(ImageURLType::Local);
      setName(source);
      return;
    }

    if (auto resolved = RelativeAssetResolver::instance()->resolve(source.toStdString())) {
      setType(ImageURLType::Local);
      setName(resolved->c_str());
      return;
    }

    if (!QIcon::fromTheme(source).isNull()) {
      setType(ImageURLType::System);
      setName(source);
      return;
    }
  }
}

ImageURL ImageURL::builtin(const QString &name) {
  ImageURL url;

  url.setType(ImageURLType::Builtin);
  url.setName(name);
  url.setFill(SemanticColor::Foreground);

  return url;
}

ImageURL ImageURL::builtin(BuiltinIcon icon) {
  if (auto name = BuiltinIconService::nameForIcon(icon)) { return ImageURL::builtin(name); }
  if (auto name = ImageURL::builtin(BuiltinIconService::unknownIcon())) { return ImageURL::builtin(name); }
  return {};
}

ImageURL::ImageURL(BuiltinIcon icon) { *this = ImageURL::builtin(icon); }

ImageURL ImageURL::favicon(const QString &domain) {
  ImageURL url;

  url.setType(ImageURLType::Favicon);
  url.setName(domain);
  url.setCacheKey(domain);

  return url;
}

ImageURL ImageURL::system(const QString &name) {
  ImageURL url;

  url.setType(ImageURLType::System);
  url.setName(name);
  url.setCachable(false); // cached internally

  return url;
}

ImageURL ImageURL::local(const QString &path) {
  ImageURL url;

  url.setType(ImageURLType::Local);
  url.setCacheKey(path);
  url.setName(path);

  return url;
}

ImageURL ImageURL::local(const std::filesystem::path &path) { return local(QString(path.c_str())); }

ImageURL ImageURL::http(const QUrl &httpUrl) {
  ImageURL url;

  url.setType(ImageURLType::Http);
  url.setName(httpUrl.toString());
  url.setCacheKey(httpUrl.toString());

  return url;
}

ImageURL ImageURL::emoji(const QString &emoji) {
  ImageURL url;

  url.setType(ImageURLType::Emoji);
  url.setName(emoji);

  return url;
}

ImageURL ImageURL::rawData(const QByteArray &data, const QString &mimeType) {
  ImageURL url;

  url.setType(ImageURLType::DataURI);
  url.setName(QString("data:%1;base64,%2").arg(mimeType).arg(data.toBase64(QByteArray::Base64UrlEncoding)));

  return url;
}

ImageURL ImageURL::fileIcon(const fs::path &path) {
  QMimeDatabase db;
  auto mime = db.mimeTypeForFile(path.c_str());
  if (auto icon = QIcon::fromTheme(mime.iconName()); !icon.isNull()) {
    return ImageURL::system(mime.iconName());
  }
  if (auto icon = QIcon::fromTheme(mime.genericIconName()); !icon.isNull()) {
    return ImageURL::system(mime.genericIconName());
  }
  return ImageURL::builtin(mime.name() == "inode/directory" ? "folder" : "blank-document");
}
