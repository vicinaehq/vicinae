#pragma once
#include "builtin_icon.hpp"
#include "extend/image-model.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtSvg/qsvgrenderer.h>
#include <libqalculate/Number.h>
#include <optional>
#include <qboxlayout.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qcoreevent.h>
#include <qdir.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qimagereader.h>
#include <qlabel.h>
#include <qlogging.h>
#include <QtSvg/QSvgRenderer>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qwidget.h>
#include "lib/emoji-detect.hpp"
#include "proto/ui.pb.h"
#include "services/asset-resolver/asset-resolver.hpp"
#include "theme.hpp"
#include "ui/omni-painter/omni-painter.hpp"

enum OmniIconType { Invalid, Builtin, Favicon, System, Http, Local, Emoji, DataURI };

static std::vector<std::pair<QString, OmniIconType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System},
    {"http", Http},       {"https", Http},       {"local", Local},
};

static std::vector<std::pair<QString, SemanticColor>> colorTints = {
    {"blue", SemanticColor::Blue},
    {"green", SemanticColor::Green},
    {"magenta", SemanticColor::Magenta},
    {"orange", SemanticColor::Orange},
    {"purple", SemanticColor::Purple},
    {"red", SemanticColor::Red},
    {"yellow", SemanticColor::Yellow},
    {"primary-text", SemanticColor::TextPrimary},
    {"secondary-text", SemanticColor::TextSecondary}};

class OmniIconUrl {
  OmniIconType _type = OmniIconType::Invalid;
  bool _isValid = false;
  QString _name;
  QSize _size;
  SemanticColor _bgTint;
  SemanticColor _fgTint;
  OmniPainter::ImageMaskType _mask = OmniPainter::ImageMaskType::NoMask;
  std::optional<QString> _fallback;
  std::optional<ColorLike> _fillColor = std::nullopt;

public:
  OmniIconUrl &circle() {
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

  static OmniIconType typeForName(const QString &name) {
    for (const auto &[n, t] : iconTypes) {
      if (name == n) return t;
    }

    return OmniIconType::Invalid;
  }

  static QString nameForType(OmniIconType type) {
    for (const auto &[n, t] : iconTypes) {
      if (t == type) return n;
    }

    return {};
  }

  bool isValid() { return _isValid; }
  operator bool() { return isValid(); }

  QString toString() const { return url().toString(); }

  std::optional<QString> fallback() const { return _fallback; }
  OmniIconUrl &withFallback(const OmniIconUrl &fallback) {
    _fallback = fallback.toString();
    return *this;
  }

  QUrl url() const {
    QUrl url;

    url.setScheme("icon");
    url.setHost(nameForType(_type));
    url.setPath("/" + _name);

    QUrlQuery query;

    if (_size.isValid()) {
      int w = _size.width();
      int h = _size.height();

      if (w == h && w >= 0) {
        query.addQueryItem("size", QString::number(w));
      } else {
        if (w >= 0) { query.addQueryItem("w", QString::number(w)); }
        if (h >= 0) { query.addQueryItem("w", QString::number(h)); }
      }
    }

    if (_fallback) query.addQueryItem("fallback", *_fallback);
    if (_bgTint != InvalidTint) query.addQueryItem("bg_tint", nameForTint(_bgTint));
    if (_fillColor) {
      if (auto tint = std::get_if<SemanticColor>(&*_fillColor); tint && *tint != InvalidTint) {
        query.addQueryItem("fill", nameForTint(*tint));
      }
    }

    url.setQuery(query);

    return url;
  }

  OmniIconType type() const { return _type; }
  const QString &name() const { return _name; }
  QSize size() const { return _size; }
  SemanticColor foregroundTint() const { return _fgTint; }
  SemanticColor backgroundTint() const { return _bgTint; }
  const std::optional<ColorLike> &fillColor() const { return _fillColor; }
  OmniPainter::ImageMaskType mask() const { return _mask; }

  void setType(OmniIconType type) { _type = type; }
  void setName(const QString &name) { _name = name; }
  void setSize(QSize size) { _size = size; }

  OmniIconUrl &setFill(const std::optional<ColorLike> &color) {
    _fillColor = color;
    return *this;
  }

  OmniIconUrl &setMask(OmniPainter::ImageMaskType mask) {
    _mask = mask;
    return *this;
  }

  OmniIconUrl &setForegroundTint(SemanticColor tint) {
    _fgTint = tint;
    return *this;
  }
  OmniIconUrl &setBackgroundTint(SemanticColor tint) {
    _bgTint = tint;
    return *this;
  }

  OmniIconUrl() : _bgTint(InvalidTint), _fgTint(InvalidTint) {}
  OmniIconUrl(const QString &s) noexcept { *this = std::move(QUrl(s)); }

  OmniIconUrl(const proto::ext::ui::Image &image) {
    using Source = proto::ext::ui::ImageSource;
    ExtensionImageModel model;

    if (image.has_color_tint()) { model.tintColor = OmniIconUrl::tintForName(image.color_tint().c_str()); }
    if (image.has_mask()) {
      switch (image.mask()) {
      case proto::ext::ui::ImageMask::Circle:
        model.mask = OmniPainter::ImageMaskType::CircleMask;
        break;
      case proto::ext::ui::ImageMask::RoundedRectangle:
        model.mask = OmniPainter::ImageMaskType::RoundedRectangleMask;
        break;
      default:
        break;
      }
    }

    switch (image.source().payload_case()) {
    case Source::kRaw:
      model.source = image.source().raw().c_str();
      break;
    case Source::kThemed: {
      auto &themed = image.source().themed();

      model.source = ThemedIconSource{.light = themed.light().c_str(), .dark = themed.dark().c_str()};
      break;
    }
    default:
      break;
    }

    if (image.has_fallback()) {
      switch (image.fallback().payload_case()) {
      case Source::kRaw:
        model.fallback = image.fallback().raw().c_str();
        break;
      default:
        break;
      }
    }

    *this = ImageLikeModel(model);
  }

  OmniIconUrl(const ImageLikeModel &imageLike)
      : _bgTint(InvalidTint), _fgTint(InvalidTint), _mask(OmniPainter::NoMask) {
    if (auto image = std::get_if<ExtensionImageModel>(&imageLike)) {
      struct {
        QString operator()(const ThemedIconSource &icon) {
          if (ThemeService::instance().theme().appearance == "light") { return icon.light; }
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
          setType(OmniIconType::Local);
          setName(url.host() + url.path());
          return;
        }

        if (url.scheme() == "data") {
          setType(OmniIconType::DataURI);
          setName(source);
          return;
        }

        if (url.scheme() == "https" || url.scheme() == "http") {
          setType(OmniIconType::Http);
          setName(source.split("://").at(1));
          return;
        }
      }

      if (isEmoji(source)) {
        setType(OmniIconType::Emoji);
        setName(source);
        return;
      }

      if (QFile(":icons/" + source + ".svg").exists()) {
        setType(OmniIconType::Builtin);
        setFill(image->tintColor.value_or(SemanticColor::TextPrimary));
        setName(source);
        return;
      }

      if (QFile(source).exists()) {
        setType(OmniIconType::Local);
        setName(source);
        return;
      }

      if (auto resolved = RelativeAssetResolver::instance()->resolve(source.toStdString())) {
        setType(OmniIconType::Local);
        setName(resolved->c_str());
        return;
      }

      if (!QIcon::fromTheme(source).isNull()) {
        setType(OmniIconType::System);
        setName(source);
        return;
      }
    }

    setName("question-mark-circle");
    setType(OmniIconType::Builtin);
    setFill(SemanticColor::TextPrimary);
  }

  bool operator==(const OmniIconUrl &rhs) const { return toString() == rhs.toString(); }

  OmniIconUrl(const QUrl &url) : _bgTint(InvalidTint), _fgTint(InvalidTint), _mask(OmniPainter::NoMask) {
    if (url.scheme() != "icon") { return; }

    _type = typeForName(url.host());

    if (_type == Invalid) { return; }

    _name = url.path().sliced(1);

    auto query = QUrlQuery(url.query());

    if (auto size = query.queryItemValue("size"); !size.isEmpty()) {
      int n = size.toInt();
      _size = {n, n};
    }

    if (auto width = query.queryItemValue("width"); !width.isEmpty()) { _size.setWidth(width.toInt()); }
    if (auto height = query.queryItemValue("height"); !height.isEmpty()) { _size.setHeight(height.toInt()); }
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

    _isValid = true;
  }
};

class BuiltinOmniIconUrl : public OmniIconUrl {
public:
  BuiltinOmniIconUrl(const QString &name, SemanticColor tint = InvalidTint) : OmniIconUrl() {
    setType(OmniIconType::Builtin);
    setName(name);
    setForegroundTint(tint);
    setFill(SemanticColor::TextPrimary);
  }
};

class FaviconOmniIconUrl : public OmniIconUrl {
public:
  FaviconOmniIconUrl(const QString &domain) : OmniIconUrl() {
    setType(OmniIconType::Favicon);
    setName(domain);
  }
};

class SystemOmniIconUrl : public OmniIconUrl {
public:
  SystemOmniIconUrl(const QString &name) : OmniIconUrl() {
    setType(OmniIconType::System);
    setName(name);
  }
};

class LocalOmniIconUrl : public OmniIconUrl {
public:
  LocalOmniIconUrl(const QString &path) : OmniIconUrl() {
    setType(OmniIconType::Local);
    setName(path);
  }
  LocalOmniIconUrl(const std::filesystem::path &path) : OmniIconUrl() {
    *this = std::move(QString(path.c_str()));
  }
};

class HttpOmniIconUrl : public OmniIconUrl {
public:
  HttpOmniIconUrl(const QUrl &url) : OmniIconUrl() {
    setType(OmniIconType::Http);
    setName(url.host() + url.path());
  }
};
