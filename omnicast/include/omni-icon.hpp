#pragma once
#include "builtin_icon.hpp"
#include "extend/image-model.hpp"
#include "favicon/favicon-service.hpp"
#include <QtConcurrent/qtconcurrentrun.h>
#include <QtSvg/qsvgrenderer.h>
#include <immintrin.h>
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
#include <qpainter.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qsize.h>
#include <qtmetamacros.h>
#include <qurl.h>
#include <qurlquery.h>
#include <qwidget.h>
#include "image-fetcher.hpp"
#include "lib/emoji-detect.hpp"
#include "theme.hpp"
#include "ui/omni-painter.hpp"

enum OmniIconType { Invalid, Builtin, Favicon, System, Http, Local, Emoji };

static std::vector<std::pair<QString, OmniIconType>> iconTypes = {
    {"favicon", Favicon}, {"omnicast", Builtin}, {"system", System},
    {"http", Http},       {"https", Http},       {"local", Local},
};

static std::vector<std::pair<QString, ColorTint>> colorTints = {{"blue", ColorTint::Blue},
                                                                {"green", ColorTint::Green},
                                                                {"magenta", ColorTint::Magenta},
                                                                {"orange", ColorTint::Orange},
                                                                {"purple", ColorTint::Purple},
                                                                {"red", ColorTint::Red},
                                                                {"yellow", ColorTint::Yellow},
                                                                {"primary-text", ColorTint::TextPrimary},
                                                                {"secondary-text", ColorTint::TextSecondary}};

class OmniIconUrl {
  OmniIconType _type;
  bool _isValid;
  QString _name;
  QSize _size;
  ColorTint _bgTint;
  ColorTint _fgTint;
  OmniPainter::ImageMaskType _mask;
  std::optional<QString> _fallback;
  std::optional<ColorLike> _fillColor = std::nullopt;

public:
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

    url.setQuery(query);

    return url;
  }

  OmniIconType type() const { return _type; }
  const QString &name() const { return _name; }
  QSize size() const { return _size; }
  ColorTint foregroundTint() const { return _fgTint; }
  ColorTint backgroundTint() const { return _bgTint; }
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
  OmniIconUrl(const ImageLikeModel &imageLike)
      : _bgTint(InvalidTint), _fgTint(InvalidTint), _mask(OmniPainter::NoMask) {
    if (auto image = std::get_if<ExtensionImageModel>(&imageLike)) {
      QUrl url(image->source);

      qDebug() << "image with source" << image->source;

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

        if (url.scheme() == "https" || url.scheme() == "http") {
          setType(OmniIconType::Http);
          setName(image->source.split("://").at(1));
          return;
        }
      }

      if (isEmoji(image->source)) {
        setType(OmniIconType::Emoji);
        setName(image->source);
        return;
      }

      if (QFile(":icons/" + image->source + ".svg").exists()) {
        setType(OmniIconType::Builtin);
        setName(image->source);
        return;
      }

      if (QFile(image->source).exists()) {
        setType(OmniIconType::Local);
        setName(image->source);
        return;
      }

      if (!QIcon::fromTheme(image->source).isNull()) {
        setType(OmniIconType::System);
        setName(image->source);
        return;
      }
    }

    setName("question-mark-circle");
    setType(OmniIconType::Builtin);
  }

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
    if (auto fill = query.queryItemValue("fill"); !fill.isEmpty()) { _fillColor = fill; }
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
  BuiltinOmniIconUrl(const QString &name, ColorTint tint = InvalidTint) : OmniIconUrl() {
    setType(OmniIconType::Builtin);
    setName(name);
    setForegroundTint(tint);
    setFill(ColorTint::TextPrimary);
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

class EmojiOmniIconRenderer : public OmniIconWidget {
  QString _emoji;
  int _pointSize;
  double _scaleHeight;
  Qt::AlignmentFlag _align;

protected:
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    QFont font = painter.font();

    font.setPointSize(qMin(width(), height()) * 0.5);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, _emoji);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);

    if (_scaleHeight != -1) { _pointSize = height() * _scaleHeight; }
  }

public:
  void setEmoji(const QString &emoji) {
    _emoji = emoji;
    update();
  }

  void setPointSize(int size) { _pointSize = size; }

  void setHeightScale(double factor) { _scaleHeight = factor; }

  void setAlignment(Qt::AlignmentFlag align) { _align = align; }

  EmojiOmniIconRenderer(const QString &emoji)
      : _emoji(emoji), _pointSize(10), _scaleHeight(-1), _align(Qt::AlignTop) {}
};

class BuiltinOmniIconRenderer : public OmniIconWidget {
  QString name;
  ColorTint _backgroundTint = InvalidTint;
  std::optional<ColorLike> _fillColor;
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
    if (size.width() * size.height() == 0) return {};

    auto &theme = ThemeService::instance();

    QPixmap canva(size);
    OmniPainter cp(&canva);

    canva.fill(Qt::transparent);
    cp.setRenderHint(QPainter::SmoothPixmapTransform, true);
    cp.setRenderHint(QPainter::Antialiasing, true);
    cp.setPen(Qt::NoPen);

    QMargins margins(0, 0, 0, 0);

    if (auto tint = _backgroundTint; tint != InvalidTint) {
      margins = {3, 3, 3, 3};
      int cornerRadius = canva.width() / 4;
      auto colorLike = theme.getTintColor(tint);

      cp.fillRect(canva.rect(), colorLike, cornerRadius);
    }

    QRect innerRect = canva.rect().marginsRemoved(margins);
    QPixmap svgPix(innerRect.size());

    svgPix.fill(Qt::transparent);

    {
      OmniPainter painter(&svgPix);
      auto svgSize = _renderer.defaultSize();
      QRect targetRect = QRect(QPoint(0, 0), svgSize.scaled(innerRect.size(), Qt::KeepAspectRatio));

      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
      _renderer.render(&painter, targetRect);

      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(svgPix.rect(), _fillColor ? *_fillColor : "#FFFFFF");
    }

    cp.drawPixmap(innerRect, svgPix);
    return canva;
  }

  int heightForWidth(int w) const override { return w; }

  QString constructResource(const QString &name) { return QString(":icons/%1.svg").arg(name); }

public:
  BuiltinOmniIconRenderer &setFillColor(const std::optional<ColorLike> &color) {
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
  BuiltinOmniIconRenderer() { setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred); }
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
  OmniPainter::ImageMaskType _mask;
  QFutureWatcher<QPixmap> watcher;

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);

    if (!_pixmap.isNull()) {
      int x = (width() - _pixmap.width()) / 2;
      int y = (height() - _pixmap.height()) / 2;
      QRect rect{x, y, _pixmap.width(), _pixmap.height()};

      painter.drawPixmap(rect, _pixmap, _mask);
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
  void setMask(OmniPainter::ImageMaskType mask) { _mask = mask; }

  LocalOmniIconWidget(const QString &path, QWidget *parent)
      : OmniIconWidget(parent), _path(path), _mask(OmniPainter::NoMask) {
    QFileInfo info(path);

    if (!info.exists()) { return; }

    connect(&watcher, &QFutureWatcher<QPixmap>::finished, this, &LocalOmniIconWidget::handleImageLoaded);
  }
};

class HttpOmniIconWidget : public OmniIconWidget {
  QString _path;
  QPixmap _pixmap;
  QFutureWatcher<QPixmap> watcher;
  OmniPainter::ImageMaskType _mask;
  QUrl url;

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);

    if (!_pixmap.isNull()) {
      int x = (width() - _pixmap.width()) / 2;
      int y = (height() - _pixmap.height()) / 2;
      QRect rect{x, y, _pixmap.width(), _pixmap.height()};

      painter.drawPixmap(rect, _pixmap, _mask);
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    recalculate();
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

    if (watcher.isRunning()) {
      watcher.cancel();
      watcher.waitForFinished();
    }

    auto reply = ImageFetcher::instance().fetch(url.toString());

    qDebug() << "fetch " << url.toString();

    connect(reply, &ImageReply::imageLoaded, this, [this, reply](QPixmap pixmap) {
      qDebug() << "got reply";
      auto targetSize = size();
      auto future = QtConcurrent::run([this, targetSize, pixmap]() {
        return pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
      });

      watcher.setFuture(future);
      reply->deleteLater();
    });

    connect(reply, &ImageReply::loadingError, this, [this, reply]() {
      qDebug() << "failed to get image";
      reply->deleteLater();
      emit imageLoadingFailed();
    });
  }

public:
  void setMask(OmniPainter::ImageMaskType type) { _mask = type; }

  HttpOmniIconWidget(const QUrl &url, QWidget *parent)
      : OmniIconWidget(parent), _mask(OmniPainter::ImageMaskType::NoMask), url(url) {
    connect(&watcher, &QFutureWatcher<QPixmap>::finished, this, &HttpOmniIconWidget::handleImageLoaded);
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
    qDebug() << "request favicon" << hostname;
    _requester = FaviconService::instance()->makeRequest(hostname, parent);
    connect(_requester, &AbstractFaviconRequest::finished, this, &FaviconOmniIconWidget::iconLoaded);
    connect(_requester, &AbstractFaviconRequest::failed, this, [this]() {
      qDebug() << "image loading failed";
      emit imageLoadingFailed();
    });
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

  void handleFailedLoading() {
    if (auto fallback = _url.fallback()) {
      setUrl(*fallback);
    } else {
      setUrl(BuiltinOmniIconUrl("question-mark-circle"));
    }
  }

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
      auto widget = new LocalOmniIconWidget(url.name(), this);

      widget->setMask(_url.mask());

      _iconWidget = widget;
    }

    else if (type == OmniIconType::Http) {
      QUrl httpUrl("https://" + url.name());
      auto widget = new HttpOmniIconWidget(httpUrl, this);

      widget->setMask(url.mask());
      _iconWidget = widget;
    }

    else if (type == OmniIconType::Emoji) {
      auto widget = new EmojiOmniIconRenderer(url.name());

      widget->setHeightScale(0.7);
      _iconWidget = widget;
    }

    if (_iconWidget) {
      connect(_iconWidget, &OmniIconWidget::imageLoaded, this, &OmniIcon::imageUpdated);
      connect(_iconWidget, &OmniIconWidget::imageLoadingFailed, this, &OmniIcon::handleFailedLoading);
      layout->addWidget(_iconWidget, 1);
    } else {
      qDebug() << "no icon widget for " << url.toString();
    }
  }

signals:
  void imageUpdated(const QPixmap &pixmap) const;
};
