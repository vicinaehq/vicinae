#pragma once

#include "builtin_icon.hpp"
#include "image-fetcher.hpp"
#include <QtSvg/qsvgrenderer.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qlabel.h>
#include <qlogging.h>
#include <QtSvg/QSvgRenderer>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qwidget.h>
#include "favicon-fetcher.hpp"
#include "theme.hpp"

enum OmniIconType { Invalid, Builtin, Favicon, System, Http, Local };

static std::vector<std::pair<QString, OmniIconType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System}, {"http", Http}, {"local", Local},
};

enum ColorTint { InvalidTint, Blue, Green, Magenta, Orange, Purple, Red, Yellow };

static std::vector<std::pair<QString, ColorTint>> colorTints = {
    {"blue", ColorTint::Blue},     {"green", ColorTint::Green},   {"magenta", ColorTint::Magenta},
    {"orange", ColorTint::Orange}, {"purple", ColorTint::Purple}, {"red", ColorTint::Red},
    {"yellow", ColorTint::Yellow},
};

struct TintLinearGradientInfo {
  QColor start;
  QColor end;
};

struct TintRadialGradientInfo {
  QColor start;
  QColor end;
};

using TintGradientInfo = std::variant<TintLinearGradientInfo, TintRadialGradientInfo>;

// clang-format off
static std::unordered_map<ColorTint, TintGradientInfo> tintGradientInfoMap = {
	{
		ColorTint::Red, 
		TintRadialGradientInfo{
       		.start = "#ff7a7c",
       		.end = "#cc2020",
		}
	}
};
// clang-format on

class OmniIconUrl {
  OmniIconType _type;
  bool _isValid;
  QString _name;
  QSize _size;
  ColorTint _bgTint;
  ColorTint _fgTint;
  QColor _fillColor;

public:
  static OmniIconUrl makeSystem(const QString &name) {
    OmniIconUrl url;

    url.setType(OmniIconType::System);
    url.setName(name);

    return url;
  }

  static ColorTint tintForName(const QString &name) {
    for (const auto &[n, t] : colorTints) {
      if (name == n) return t;
    }

    return ColorTint::InvalidTint;
  }

  static QString nameForTint(ColorTint type) {
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

  QString toString() { return url().toString(); }

  QUrl url() {
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

    url.setQuery(query);

    return url;
  }

  OmniIconType type() const { return _type; }
  const QString &name() const { return _name; }
  QSize size() const { return _size; }
  ColorTint foregroundTint() const { return _fgTint; }
  ColorTint backgroundTint() const { return _bgTint; }
  const QColor &fillColor() const { return _fillColor; }

  void setType(OmniIconType type) { _type = type; }
  void setName(const QString &name) { _name = name; }
  void setSize(QSize size) { _size = size; }

  OmniIconUrl &setFill(const QColor &color) {
    _fillColor = color;
    return *this;
  }

  OmniIconUrl &setForegroundTint(ColorTint tint) {
    _fgTint = tint;
    return *this;
  }
  OmniIconUrl &setBackgroundTint(ColorTint tint) {
    _bgTint = tint;
    return *this;
  }

  OmniIconUrl() noexcept {}
  OmniIconUrl(const QString &s) noexcept { *this = std::move(QUrl(s)); }
  OmniIconUrl(const QUrl &url) noexcept {
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
    if (auto fill = query.queryItemValue("fill"); !fill.isEmpty()) { _fillColor = fill; }

    _isValid = true;
  }
};

class BuiltinOmniIconUrl : public OmniIconUrl {
  QColor _fillColor;

public:
  BuiltinOmniIconUrl(const QString &name, ColorTint tint = InvalidTint) : OmniIconUrl() {
    setType(OmniIconType::Builtin);
    setName(name);
    setForegroundTint(tint);
    setFill(ThemeService::instance().theme().colors.text);
  }

  BuiltinOmniIconUrl &setFillColor(const QColor &color) {
    _fillColor = color;
    return *this;
  }
};

class FaviconOmniIconUrl : public OmniIconUrl {
public:
  FaviconOmniIconUrl(const QString &domain) : OmniIconUrl() {
    setType(OmniIconType::Favicon);
    setName(domain);
  }
};

class OmniIcon : public QWidget {
  Q_OBJECT

  QSvgRenderer renderer;

  QPixmap _pixmap;
  QColor _background;
  int _backgroundRadius = 6;
  OmniIconUrl _url;
  std::optional<TintGradientInfo> _gradientInfo;

  void setDefaultIcon(QSize size) { setIcon(BuiltinIconService::unknownIcon(), size); }

  void setPixmap(const QPixmap &pixmap) {
    emit imageUpdated(pixmap);
    _pixmap = pixmap;
    update();
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculate();
  }

  QPixmap fillMonochrome(const QPixmap &source, const QColor &oldColor, const QColor &newColor) {
    QImage image = source.toImage();

    // Get the RGB value for the new color
    QRgb newRgb = newColor.rgb();

    // White color in RGB (fully opaque)
    QRgb whiteRgb = oldColor.rgb();

    // Iterate through all pixels in the image
    for (int y = 0; y < image.height(); ++y) {
      for (int x = 0; x < image.width(); ++x) {
        QRgb pixelColor = image.pixel(x, y);

        // Check if the pixel is white (ignoring alpha)
        if ((pixelColor & 0x00FFFFFF) == (whiteRgb & 0x00FFFFFF)) {
          // Preserve the original alpha channel
          QRgb newColorWithAlpha = (pixelColor & 0xFF000000) | (newRgb & 0x00FFFFFF);
          image.setPixel(x, y, newColorWithAlpha);
        }
      }
    }

    return QPixmap::fromImage(image);
  }

  void recalculate() {
    if (_url.type() == OmniIconType::Builtin) {
      auto pixRect = rect().marginsRemoved(contentsMargins());
      QPixmap drawingSurface(pixRect.size());
      drawingSurface.fill(Qt::transparent);

      QPainter painter(&drawingSurface);

      renderer.render(&painter);

      if (auto fill = _url.fillColor(); fill.isValid()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(drawingSurface.rect(), fill);
      }

      setPixmap(drawingSurface);
    }
  }

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    if (_gradientInfo) {
      QBrush brush;

      if (auto info = std::get_if<TintRadialGradientInfo>(&*_gradientInfo)) {
        QRadialGradient gradient(rect().center(), rect().width() / 2.0);
        gradient.setColorAt(0, info->start); // Center color
        gradient.setColorAt(1, info->end);   // Edge color
        gradient.setSpread(QGradient::PadSpread);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
        brush = gradient;
      } else if (auto info = std::get_if<TintLinearGradientInfo>(&*_gradientInfo)) {
        QLinearGradient gradient;

        gradient.setColorAt(0, info->start);
        gradient.setColorAt(1, info->end);
        brush = gradient;
      }

      painter.setBrush(brush);
      painter.setPen(Qt::NoPen);
      int cornerRadius = rect().width() / 4;

      painter.drawRoundedRect(rect(), cornerRadius, cornerRadius);
    }

    auto pixRect = rect().marginsRemoved(contentsMargins());
    auto scaled = _pixmap.scaled(pixRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    painter.drawPixmap(pixRect, scaled);
  }

public:
  OmniIcon(QWidget *parent = nullptr) : QWidget(parent) { setContentsMargins(0, 0, 0, 0); }
  ~OmniIcon() {}

  void setUrl(const OmniIconUrl &url) {
    _url = url;
    setContentsMargins(0, 0, 0, 0);
    _gradientInfo.reset();

    auto type = url.type();
    // take widget size if no icon size is specified
    QSize iconSize = url.size().isValid() ? url.size() : size();

    if (url.backgroundTint() != InvalidTint) {
      if (auto it = tintGradientInfoMap.find(url.backgroundTint()); it != tintGradientInfoMap.end()) {
        setContentsMargins(3, 3, 3, 3);
        _gradientInfo = it->second;
      }
    }

    if (type == OmniIconType::Favicon) {
      QPixmap pm;
      auto hostname = url.name();

      if (QPixmapCache::find(hostname, &pm)) {
        setPixmap(pm.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        return;
      }

      auto reply = FaviconFetcher::fetchFavicon(hostname, iconSize);

      connect(reply, &FaviconReply::failed, this, [this, iconSize, reply]() {
        setUrl(OmniIconUrl("icon://omnicast/question-mark-circle"));
        reply->deleteLater();
      });

      connect(reply, &FaviconReply::finished, this, [this, iconSize, reply](QPixmap pixmap) {
        setPixmap(pixmap.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        reply->deleteLater();
      });

      return;
    }

    if (type == OmniIconType::System) { auto icon = QIcon::fromTheme(url.name()); }

    if (type == OmniIconType::Builtin) {
      auto icon = QString(":icons/%1.svg").arg(url.name());

      if (!renderer.load(icon)) {
        setDefaultIcon(iconSize);
      } else {
        // recalculate();
      }
      return;
    }

    auto icon = QIcon::fromTheme(url.name());

    if (icon.isNull()) {
      setDefaultIcon(iconSize);
      return;
    }

    setPixmap(icon.pixmap(iconSize));
  }

  void setIcon(const QString &id, QSize size, const QString &fallback = "") {
    if (size.isValid()) setFixedSize(size);

    auto ss = id.split(":");
    QString type, name;

    if (ss.size() == 2) {
      type = ss.at(0);

      if (type.isEmpty()) {
        name = ":" + ss.at(1);
      } else {
        name = ss.at(1);
      }
    } else {
      name = ss.at(0);
    }

    if (type == "favicon") {
      QPixmap pm;

      if (QPixmapCache::find(name, &pm)) {
        setPixmap(pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        return;
      }

      auto reply = FaviconFetcher::fetchFavicon(name, size);

      connect(reply, &FaviconReply::failed, this, [this, fallback, size, reply]() {
        if (!fallback.isEmpty()) {
          setIcon(fallback, size);
        } else {
          setDefaultIcon(size);
        }

        reply->deleteLater();
      });

      connect(reply, &FaviconReply::finished, this, [this, size, reply](QPixmap pixmap) {
        setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        reply->deleteLater();
      });

      return;
    }

    if (type == "url") {
      QUrl url(name);

      if (url.isValid()) {
        auto reply = ImageFetcher::instance().fetch(url.toString(), {});

        connect(reply, &ImageReply::imageLoaded, this,
                [this, size](QPixmap pixmap) { setPixmap(pixmap.scaled(size)); });
      }

      return;
    }

    auto icon = QIcon::fromTheme(name);

    if (icon.isNull()) {
      if (fallback.isEmpty())
        return setDefaultIcon(size);
      else
        return setIcon(fallback, size);
    }

    setPixmap(icon.pixmap(size));
  }

signals:
  void imageUpdated(QPixmap pixmap);
};
