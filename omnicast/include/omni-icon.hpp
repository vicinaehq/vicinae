#pragma once

#include "builtin_icon.hpp"
#include "favicon/favicon-service.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtSvg/qsvgrenderer.h>
#include <qboxlayout.h>
#include <qbrush.h>
#include <qcolor.h>
#include <qdir.h>
#include <qevent.h>
#include <qfuturewatcher.h>
#include <qimagereader.h>
#include <qlabel.h>
#include <qlogging.h>
#include <QtSvg/QSvgRenderer>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qwidget.h>
#include "theme.hpp"

enum OmniIconType { Invalid, Builtin, Favicon, System, Http, Local };

static std::vector<std::pair<QString, OmniIconType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System}, {"http", Http}, {"local", Local},
};

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

  QString toString() const { return url().toString(); }

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

class AbstractOmniIconRenderer {
public:
  virtual QPixmap render(QSize size) = 0;
};

class OmniIconWidget : public QWidget {
  Q_OBJECT

  QSize sizeHint() const override {
    if (parentWidget()) { return parentWidget()->size(); }
    return {};
  }

public:
  OmniIconWidget(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void imageLoaded(const QPixmap &data) const;
  void imageLoadingFailed() const;
};

class BuiltinOmniIconRenderer : public OmniIconWidget {
  QString name;
  ColorTint _backgroundTint = InvalidTint;
  QColor _fillColor;
  QSvgRenderer _renderer;
  QPixmap _pixmap;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.drawPixmap(pos(), _pixmap);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    _pixmap = render(event->size());
    emit imageLoaded(_pixmap);
  }

  QPixmap render(QSize size) {
    auto &theme = ThemeService::instance();
    QPixmap canva(size);
    QPainter cp(&canva);

    canva.fill(Qt::transparent);
    cp.setRenderHint(QPainter::SmoothPixmapTransform, true);
    cp.setRenderHint(QPainter::Antialiasing, true);
    cp.setPen(Qt::NoPen);

    QMargins margins(0, 0, 0, 0);

    if (auto tint = _backgroundTint; tint != InvalidTint) {
      margins = {3, 3, 3, 3};
      int cornerRadius = canva.width() / 4;
      auto colorLike = theme.getTintColor(tint);

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

    QRect innerRect = canva.rect().marginsRemoved(margins);
    QPixmap svgPix(innerRect.size());

    svgPix.fill(Qt::transparent);

    {
      QPainter painter(&svgPix);
      auto svgSize = _renderer.defaultSize();
      QRect targetRect = QRect(QPoint(0, 0), svgSize.scaled(innerRect.size(), Qt::KeepAspectRatio));

      _renderer.render(&painter, targetRect);

      if (auto fill = _fillColor; _fillColor.isValid()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(svgPix.rect(), fill);
      }
    }

    cp.drawPixmap(innerRect, svgPix);
    return canva;
  }

  QString constructResource(const QString &name) { return QString(":icons/%1.svg").arg(name); }

public:
  BuiltinOmniIconRenderer &setFillColor(const QColor &color) {
    _fillColor = color;
    return *this;
  }
  BuiltinOmniIconRenderer &setBackgroundTaint(ColorTint color) {
    _backgroundTint = color;
    return *this;
  }
  BuiltinOmniIconRenderer(const QString &name, QWidget *parent = nullptr) : OmniIconWidget(parent) {
    if (!_renderer.load(constructResource(name))) { emit imageLoadingFailed(); }
  }
};

class OmniSystemIconWidget : public OmniIconWidget {
  QPixmap _pixmap;
  QIcon _icon;
  QString _name;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.drawPixmap(0, 0, _pixmap);
  }

  QSize bestSize() {
    QSize best = size();

    for (const auto &size : _icon.availableSizes()) {
      if (size.width() * size.height() > best.width() * best.height()) { best = size; }
    }

    return best;
  }

  void recalculate() {
    if (!size().isValid()) return;

    _pixmap = _icon.pixmap(bestSize()).scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
    emit imageLoaded(_pixmap);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculate();
  }

public:
  OmniSystemIconWidget(const QString &name, QWidget *parent = nullptr) : OmniIconWidget(parent), _name(name) {
    _icon = QIcon(name);
    if (_icon.isNull()) { _icon = QIcon::fromTheme(name); }
    if (_icon.isNull()) { _icon = QIcon(":icons/question-mark-circle"); }
    if (_icon.isNull()) { emit imageLoadingFailed(); }
  }
};

class LocalOmniIconWidget : public OmniIconWidget {
  QString _path;
  QPixmap _pixmap;
  QFutureWatcher<QPixmap> watcher;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    if (!_pixmap.isNull()) {
      int x = (width() - _pixmap.width()) / 2;
      int y = (height() - _pixmap.height()) / 2;

      painter.drawPixmap(x, y, _pixmap);
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculate();
  }

  QPixmap loadImage(const QString &path, QSize size) {
    QImageReader reader(path);
    QSize originalSize = reader.size();
    bool isDownScalable = originalSize.height() > size.height() || originalSize.width() > size.width();

    if (originalSize.isValid() && isDownScalable) {
      reader.setScaledSize(originalSize.scaled(size, Qt::KeepAspectRatio));
    }

    return QPixmap::fromImage(reader.read());
  }

  void handleImageLoaded() {
    if (watcher.isCanceled()) { return; }

    if (auto pix = watcher.result(); !pix.isNull()) {
      _pixmap = pix;
      update();
      emit imageLoaded(_pixmap);
    } else {
      emit imageLoadingFailed();
    }
  }

  void recalculate() {
    if (!size().isValid()) return;

    auto imageSize = size();

    if (watcher.isRunning()) {
      watcher.cancel();
      watcher.waitForFinished();
    }

    auto future = QtConcurrent::run([this, imageSize]() { return loadImage(_path, imageSize); });

    watcher.setFuture(future);
  }

public:
  LocalOmniIconWidget(const QString &path, QWidget *parent) : OmniIconWidget(parent), _path(path) {
    QFileInfo info(path);

    if (!info.exists()) { return; }

    connect(&watcher, &QFutureWatcher<QPixmap>::finished, this, &LocalOmniIconWidget::handleImageLoaded);
  }
};

class FaviconOmniIconWidget : public OmniIconWidget {
  QPixmap _favicon;
  QPixmap _pixmap;
  AbstractFaviconRequest *_requester;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.drawPixmap(pos(), _pixmap);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculate();
  }

  void iconLoaded(QPixmap pixmap) {
    _favicon = pixmap;

    recalculate();
  }

  void recalculate() {
    if (!size().isValid() || _favicon.isNull()) return;

    _pixmap = _favicon.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    emit imageLoaded(_pixmap);
    update();
  }

public:
  FaviconOmniIconWidget(const QString &hostname, QWidget *parent) : OmniIconWidget(parent) {
    _requester = FaviconService::instance()->makeRequest(hostname, parent);
    connect(_requester, &AbstractFaviconRequest::finished, this, &FaviconOmniIconWidget::iconLoaded);
    connect(_requester, &AbstractFaviconRequest::failed, this, [this]() { emit imageLoadingFailed(); });
    _requester->start();
  }
};

enum OmniIconSizeMode {
  SizeModeFill,
  SizeModeFixed,
};

class OmniIcon : public QWidget {
  Q_OBJECT

  OmniIconUrl _url;
  OmniIconWidget *_iconWidget = nullptr;
  QSize _fixedSize;
  OmniIconSizeMode _sizeMode = SizeModeFill;
  QVBoxLayout *layout;

public:
  OmniIcon(const OmniIconUrl &url, QWidget *parent = nullptr) : QWidget(parent), layout(new QVBoxLayout) {
    setUrl(url);
  }
  OmniIcon(QWidget *parent = nullptr) : QWidget(parent), layout(new QVBoxLayout) {
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }
  ~OmniIcon() {}

  void setUrl(const OmniIconUrl &url) {
    if (_iconWidget) {
      layout->takeAt(0);
      _iconWidget->deleteLater();
    }

    _url = url;

    auto type = url.type();

    if (type == OmniIconType::Favicon) {
      _iconWidget = new FaviconOmniIconWidget(url.name(), this);
    }

    else if (type == OmniIconType::System) {
      _iconWidget = new OmniSystemIconWidget(url.name(), this);
    }

    else if (type == OmniIconType::Builtin) {
      auto iconWidget = new BuiltinOmniIconRenderer(url.name(), this);

      iconWidget->setBackgroundTaint(url.backgroundTint()).setFillColor(url.fillColor());
      _iconWidget = iconWidget;
    }

    else if (type == OmniIconType::Local) {
      _iconWidget = new LocalOmniIconWidget(url.name(), this);
    }

    if (_iconWidget) {
      connect(_iconWidget, &OmniIconWidget::imageLoaded, this, &OmniIcon::imageUpdated);
      layout->addWidget(_iconWidget, 1);
    } else {
      qDebug() << "no icon widget for " << url.toString();
    }
  }

signals:
  void imageUpdated(const QPixmap &pixmap) const;
};
