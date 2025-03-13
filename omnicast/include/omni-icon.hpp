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
#include "timer.hpp"

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

  OmniIconUrl() : _bgTint(InvalidTint), _fgTint(InvalidTint) {}
  OmniIconUrl(const QString &s) noexcept { *this = std::move(QUrl(s)); }
  OmniIconUrl(const QUrl &url) : _bgTint(InvalidTint), _fgTint(InvalidTint) {
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
};

class OmniIcon : public QWidget {
  Q_OBJECT

  QSvgRenderer renderer;
  QPixmap _filePixmap;
  QPixmap _pixmap;
  OmniIconUrl _url;

  void setDefaultIcon(QSize size) { setIcon(BuiltinIconService::unknownIcon(), size); }

  void setPixmap(const QPixmap &pixmap) {
    emit imageUpdated(pixmap);
    _pixmap = pixmap;
    update();
  }

  void resizeEvent(QResizeEvent *event) override {
    qDebug() << "resize omni icon" << event->size();
    if (event->size().width() == 0 || event->size().height() == 0) { return; }
    recalculate();
  }

  ColorLike getThemeTintColor(const ThemeInfo &info, ColorTint tint) {
    switch (tint) {
    case ColorTint::Blue:
      return info.colors.blue;
    case ColorTint::Green:
      return info.colors.green;
    case ColorTint::Magenta:
      return info.colors.magenta;
    case ColorTint::Orange:
      return info.colors.orange;
    case ColorTint::Purple:
      return info.colors.purple;
    case ColorTint::Red:
      return info.colors.red;
    case ColorTint::Yellow:
      return info.colors.yellow;
    default:
      break;
    }

    return {};
  }

  QSize sizeHint() const override {
    if (auto sz = _url.size(); sz.isValid()) { return sz; }
    if (auto w = parentWidget(); w) { return w->size(); }

    return {};
  }

  void recalculate() {
    qDebug() << "recalculate for size" << size();
    auto &theme = ThemeService::instance().theme();
    QPixmap canva(size());
    QPainter cp(&canva);

    canva.fill(Qt::transparent);
    cp.setRenderHint(QPainter::SmoothPixmapTransform, true);
    cp.setRenderHint(QPainter::Antialiasing, true);
    cp.setPen(Qt::NoPen);

    if (auto tint = _url.backgroundTint(); tint != InvalidTint) {
      setContentsMargins(3, 3, 3, 3);

      int cornerRadius = canva.width() / 4;
      auto colorLike = getThemeTintColor(theme, tint);

      if (auto color = std::get_if<QColor>(&colorLike)) {
        cp.setBrush(*color);
        cp.drawRoundedRect(canva.rect(), cornerRadius, cornerRadius);
      } else if (auto lgrad = std::get_if<ThemeLinearGradient>(&colorLike)) {
        QLinearGradient gradient;

        for (int i = 0; i != lgrad->points.size(); ++i) {
          gradient.setColorAt(i, lgrad->points[i]);
        }

        cp.setBrush(gradient);
        cp.drawRoundedRect(canva.rect(), cornerRadius, cornerRadius);
      } else if (auto rgrad = std::get_if<ThemeRadialGradient>(&colorLike)) {
        QRadialGradient gradient(canva.rect().center(), canva.rect().width() / 2.0);

        gradient.setSpread(QGradient::PadSpread);
        gradient.setCoordinateMode(QGradient::ObjectBoundingMode);

        for (int i = 0; i != rgrad->points.size(); ++i) {
          gradient.setColorAt(i, rgrad->points[i]);
        }

        cp.setBrush(gradient);
        cp.drawRoundedRect(canva.rect(), cornerRadius, cornerRadius);
      }
    }

    QRect innerRect = canva.rect().marginsRemoved(contentsMargins());

    if (_url.type() == OmniIconType::Builtin) {
      QPixmap svgPix(innerRect.size());

      svgPix.fill(Qt::transparent);

      {
        QPainter painter(&svgPix);

        renderer.render(&painter);

        if (auto fill = _url.fillColor(); fill.isValid()) {
          painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
          painter.fillRect(svgPix.rect(), fill);
        }
      }

      cp.drawPixmap(innerRect, svgPix);
      setPixmap(canva);
      return;
    }

    if (_url.type() == OmniIconType::Local) {
      auto isz = innerRect.size();

      Timer timer;
      QPixmap scaled = _filePixmap.scaled(innerRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

      int x = (isz.width() - scaled.width()) / 2;
      int y = (isz.height() - scaled.height()) / 2;

      qDebug() << "drawing" << scaled.size();

      cp.drawPixmap(x, y, scaled);
      timer.time("scaling pixmap");
      updateGeometry();
      setPixmap(canva);
      return;
    }

    cp.drawPixmap(innerRect, _pixmap);
    setPixmap(canva);
  }

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(rect(), _pixmap);
  }

public:
  OmniIcon(const OmniIconUrl &url, QWidget *parent = nullptr) : QWidget(parent) { setUrl(url); }
  OmniIcon(QWidget *parent = nullptr) : QWidget(parent) {
    connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { recalculate(); });
    setContentsMargins(0, 0, 0, 0);
  }
  ~OmniIcon() {}

  void setUrl(const OmniIconUrl &url) {
    _url = url;

    auto type = url.type();
    // take widget size if no icon size is specified
    QSize iconSize = url.size().isValid() ? url.size() : size();

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
        recalculate();
      }
      return;
    }

    if (type == OmniIconType::Local) {
      Timer timer;
      auto pix = QPixmap(url.name());

      if (pix.isNull()) {
        setDefaultIcon(iconSize);
        return;
      }

      qDebug() << "loading local image" << url.name() << "size" << pix.size();

      timer.time("create local pixmap");
      _filePixmap = pix;

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
