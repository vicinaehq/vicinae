#include "builtin_icon.hpp"
#include "emoji/emoji.hpp"
#include "extend/image-model.hpp"
#include "services/asset-resolver/asset-resolver.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "theme/theme-file.hpp"
#include <qdir.h>
#include <qstringview.h>
#include <QFile>
#include <QIcon>
#include <qurlquery.h>
#include "url.hpp"

namespace fs = std::filesystem;

ImageURL &ImageURL::setFill(const std::optional<ColorLike> &color) {
  _fillColor = color;
  return *this;
}

ImageURL &ImageURL::setMask(OmniPainter::ImageMaskType mask) {
  _mask = mask;
  return *this;
}

ImageURL &ImageURL::setBackgroundTint(const ColorLike &tint) {
  _bgTint = tint;
  return *this;
}

ImageURLType ImageURL::type() const { return _type; }
const QString &ImageURL::name() const { return _name; }
std::optional<ColorLike> ImageURL::backgroundTint() const { return _bgTint; }
const std::optional<ColorLike> &ImageURL::fillColor() const { return _fillColor; }
OmniPainter::ImageMaskType ImageURL::mask() const { return _mask; }

ImageURL &ImageURL::withFallback(const ImageURL &fallback) {
  _fallback = fallback.toString();
  return *this;
}

static QString resolveThemedLocalPath(const QString &path) {
  QString const suffix =
      QStringLiteral("@") + (ThemeService::instance().theme().isLight() ? "light" : "dark");

  qsizetype const baseStart = path.lastIndexOf('/') + 1;
  qsizetype const dotPos = path.indexOf('.', baseStart);
  QString const themed = dotPos == -1 ? path + suffix : path.left(dotPos) + suffix + path.mid(dotPos);

  if (QFile::exists(themed)) return themed;
  return path;
}

ImageURL ImageURL::resolved() const {
  ImageURL out = *this;
  if (auto fill = fillColor())
    out.setFill(OmniPainter::resolveColor(*fill));
  else if (type() == ImageURLType::Builtin)
    out.setFill(ThemeService::instance().theme().resolve(SemanticColor::Foreground));
  if (auto bg = backgroundTint()) out.setBackgroundTint(OmniPainter::resolveColor(*bg));
  if (out.type() == ImageURLType::Local) out.setName(resolveThemedLocalPath(out.name()));
  return out;
}

QUrl ImageURL::url() const {
  QUrl url;

  url.setScheme("icon");
  url.setHost(nameForType(_type));
  url.setPath("/" + _name);

  QUrlQuery query;

  if (_fallback) query.addQueryItem("fallback", *_fallback);
  if (_bgTint) query.addQueryItem("bg_tint", OmniPainter::serializeColor(*_bgTint));
  if (_fillColor) query.addQueryItem("fill", OmniPainter::serializeColor(_fillColor.value()));
  if (_mask == OmniPainter::CircleMask)
    query.addQueryItem("mask", "circle");
  else if (_mask == OmniPainter::RoundedRectangleMask)
    query.addQueryItem("mask", "roundedRectangle");

  url.setQuery(query);

  return url;
}

void ImageURL::setType(ImageURLType type) { _type = type; }
void ImageURL::setName(const QString &name) { _name = name; }

ImageURL::ImageURL(const QString &s) noexcept { *this = std::move(QUrl(s)); }

ImageURL::ImageURL() = default;

ImageURL::ImageURL(const QUrl &url) {
  if (url.scheme() != "icon") { return; }

  _type = typeForName(url.host());

  if (_type == Invalid) { return; }

  _name = url.path().sliced(1);

  auto query = QUrlQuery(url.query());

  if (auto bgTint = query.queryItemValue("bg_tint"); !bgTint.isEmpty()) {
    if (auto tint = tintForName(bgTint); tint != SemanticColor::InvalidTint)
      _bgTint = tint;
    else
      _bgTint = QColor(bgTint);
  }
  if (auto fill = query.queryItemValue("fill"); !fill.isEmpty()) {
    if (auto tint = tintForName(fill); tint != SemanticColor::InvalidTint)
      _fillColor = tint;
    else
      _fillColor = QColor(fill);
  }
  if (auto fallback = query.queryItemValue("fallback"); !fallback.isEmpty()) { _fallback = fallback; }
  if (auto mask = query.queryItemValue("mask"); !mask.isEmpty()) {
    if (mask == "circle")
      _mask = OmniPainter::ImageMaskType::CircleMask;
    else if (mask == "roundedRectangle")
      _mask = OmniPainter::ImageMaskType::RoundedRectangleMask;
  }

  _isValid = true;
}

ImageURL::ImageURL(const ImageLikeModel &imageLike) {
  if (auto image = std::get_if<ExtensionImageModel>(&imageLike)) {
    struct {
      QString operator()(const ThemedIconSource &icon) {
        if (ThemeService::instance().theme().isLight()) { return icon.light; }
        return icon.dark;
      }
      QString operator()(const QString &icon) { return icon; }
    } visitor;

    auto source = std::visit(visitor, image->source);

    QUrl const url(source);

    if (auto fallback = image->fallback) {
      withFallback(ImageLikeModel(ExtensionImageModel{.source = *fallback}));
    }

    if (auto tintColor = image->tintColor) { setFill(tintColor); }
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

  if (auto fileIcon = std::get_if<ExtensionFileIconModel>(&imageLike)) {
    *this = ImageURL::fileIcon(fileIcon->file);
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

  return url;
}

ImageURL ImageURL::system(const QString &name) {
  ImageURL url;

  url.setType(ImageURLType::System);
  url.setName(name);

  return url;
}

ImageURL ImageURL::local(const QString &path) {
  ImageURL url;

  url.setType(ImageURLType::Local);
  url.setName(path);

  return url;
}

ImageURL ImageURL::local(const std::filesystem::path &path) { return local(QString(path.c_str())); }

ImageURL ImageURL::macBundle(const std::filesystem::path &bundlePath) {
  ImageURL url;
  QString const name = QString::fromStdString(bundlePath.string());

  url.setType(ImageURLType::MacBundle);
  url.setName(name);

  return url;
}

ImageURL ImageURL::http(const QUrl &httpUrl) {
  ImageURL url;

  url.setType(ImageURLType::Http);
  url.setName(httpUrl.toString());

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
  url.setName(QString("data:%1;base64,%2").arg(mimeType).arg(data.toBase64()));

  return url;
}

ImageURL ImageURL::fileIcon(const fs::path &path) {
  ImageURL url;

  url.setType(ImageURLType::FileIcon);
  url.setName(QString::fromStdString(path.string()));

  return url;
}
