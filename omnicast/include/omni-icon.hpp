#pragma once
#include "builtin_icon.hpp"
#include "extend/image-model.hpp"
#include "favicon/favicon-service.hpp"
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
#include "network-manager.hpp"
#include "proto/ui.pb.h"
#include "services/asset-resolver/asset-resolver.hpp"
#include "theme.hpp"
#include "ui/omni-painter.hpp"

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
        qDebug() << "url" << url;

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
  SemanticColor _backgroundTint = InvalidTint;
  std::optional<ColorLike> _fillColor;
  QSvgRenderer _renderer;
  QPixmap _pixmap;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(rect(), _pixmap);
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    _pixmap = render(event->size());
    emit imageLoaded(_pixmap);
  }

  QPixmap render(QSize size) {
    if (size.width() * size.height() == 0) return {};

    qreal devicePixelRatio = qApp->devicePixelRatio();
    QSize deviceSize = size * devicePixelRatio;
    QPixmap canva(deviceSize);
    auto &theme = ThemeService::instance();

    canva.fill(Qt::transparent);
    // canva.setDevicePixelRatio(devicePixelRatio);

    OmniPainter cp(&canva);

    cp.setRenderHint(QPainter::SmoothPixmapTransform, true);
    cp.setRenderHint(QPainter::Antialiasing, true);
    cp.setPen(Qt::NoPen);

    QMargins margins(0, 0, 0, 0);

    if (auto tint = _backgroundTint; tint != InvalidTint) {
      int margin = qRound(3 * devicePixelRatio);

      margins = {margin, margin, margin, margin};
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

    svgPix.setDevicePixelRatio(devicePixelRatio);

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
  BuiltinOmniIconRenderer &setBackgroundTaint(SemanticColor color) {
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

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(rect(), _pixmap);
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

    QPixmap canva(size() * qApp->devicePixelRatio());
    QPixmap icon =
        _icon.pixmap(bestSize()).scaled(canva.rect().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    icon.setDevicePixelRatio(1);

    // icon.setDevicePixelRatio(qApp->devicePixelRatio());
    canva.fill(Qt::transparent);

    QPainter cp(&canva);

    cp.drawPixmap(canva.rect(), icon);
    _pixmap = canva;

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

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    qDebug() << "render local image" << _pixmap;

    if (!_pixmap.isNull()) {
      int x = (width() - _pixmap.width()) / 2;
      int y = (height() - _pixmap.height()) / 2;
      // QRect rect{x, y, _pixmap.width(), _pixmap.height()};

      painter.drawPixmap(rect(), _pixmap, _mask);
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

    auto pix = QPixmap::fromImageReader(&reader);

    pix.setDevicePixelRatio(1);

    return pix;
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

    QSize deviceSize = size() * qApp->devicePixelRatio();
    auto future = QtConcurrent::run([this, deviceSize]() { return loadImage(_path, deviceSize); });

    watcher.setFuture(future);
  }

public:
  void setMask(OmniPainter::ImageMaskType mask) { _mask = mask; }

  LocalOmniIconWidget(const QString &path, QWidget *parent)
      : OmniIconWidget(parent), _path(path), _mask(OmniPainter::NoMask) {
    QFileInfo info(path);

    if (!info.exists()) { emit imageLoadingFailed(); }

    connect(&watcher, &QFutureWatcher<QPixmap>::finished, this, &LocalOmniIconWidget::handleImageLoaded);
  }
};

class HttpOmniIconWidget : public OmniIconWidget {
  QString _path;
  QPixmap _pixmap;
  QNetworkReply *m_reply = nullptr;
  QFutureWatcher<QPixmap> watcher;
  OmniPainter::ImageMaskType _mask = OmniPainter::ImageMaskType::NoMask;
  QSize m_lastLoadedForSize;
  QUrl url;

  void paintEvent(QPaintEvent *event) override {
    OmniPainter painter(this);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (!_pixmap.isNull()) {
      int x = (width() - _pixmap.width()) / 2;
      int y = (height() - _pixmap.height()) / 2;
      // QRect rect{x, y, _pixmap.width(), _pixmap.height()};

      painter.drawPixmap(rect(), _pixmap, _mask);
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    qDebug() << "http image resize" << event->size();
    if (event->size() == m_lastLoadedForSize) {
      qDebug() << "same size, not doing anything";
      return;
    }

    QWidget::resizeEvent(event);
    recalculate();
  }

  void handleImageLoaded() {
    if (watcher.isCanceled()) { return; }

    m_lastLoadedForSize = size();

    if (auto pix = watcher.result(); !pix.isNull()) {
      pix.setDevicePixelRatio(1);
      _pixmap = pix;
      update();
      emit imageLoaded(_pixmap);
    } else {
      qDebug() << "pixmap is null!";
      emit imageLoadingFailed();
    }
  }

  static QPixmap loadImage(QByteArray data, QSize size) {
    QBuffer buffer(&data);

    buffer.open(QIODevice::ReadOnly);

    QImageReader reader(&buffer);

    if (!reader.canRead()) { return {}; }

    QSize originalSize = reader.size();
    bool isDownScalable = originalSize.height() > size.height() || originalSize.width() > size.width();

    if (originalSize.isValid() && isDownScalable) {
      reader.setScaledSize(originalSize.scaled(size, Qt::KeepAspectRatio));
    }

    return QPixmap::fromImage(reader.read());
  }

  void recalculate() {
    if (!size().isValid()) return;

    if (watcher.isRunning()) {
      watcher.cancel();
      watcher.waitForFinished();
    }

    if (m_reply) {
      disconnect(m_reply, nullptr, this, nullptr);
      m_reply->abort();
    }

    QNetworkRequest request(url);

    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    m_reply = NetworkManager::instance()->manager()->get(request);

    connect(m_reply, &QNetworkReply::finished, this, [this]() {
      if (m_reply->error() == QNetworkReply::NoError) {
        auto data = m_reply->readAll();
        QSize deviceSize = size() * qApp->devicePixelRatio();
        auto future = QtConcurrent::run(&HttpOmniIconWidget::loadImage, std::move(data), deviceSize);

        watcher.setFuture(future);
      } else {
        // NetworkManager::instance()->manager()->cache()->remove(url);
        qCritical() << "Failed to load image" << m_reply->errorString();
      }

      qCritical() << "destroy network request";
      m_reply->deleteLater();
      m_reply = nullptr;
    });

    qDebug() << "fetch " << url.toString();
  }

public:
  void setMask(OmniPainter::ImageMaskType type) { _mask = type; }

  HttpOmniIconWidget(const QUrl &url, QWidget *parent)
      : OmniIconWidget(parent), _mask(OmniPainter::ImageMaskType::NoMask), url(url) {
    qDebug() << "init http image for" << url;
    connect(&watcher, &QFutureWatcher<QPixmap>::finished, this, &HttpOmniIconWidget::handleImageLoaded);
  }

  ~HttpOmniIconWidget() {
    if (m_reply) { m_reply->abort(); }
  }
};

class FaviconOmniIconWidget : public OmniIconWidget {
  QPixmap _favicon;
  QPixmap _pixmap;
  AbstractFaviconRequest *_requester;

  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);

    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(rect(), _pixmap);
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

    QSize deviceSize = size() * qApp->devicePixelRatio();

    _pixmap = _favicon.scaled(deviceSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    _pixmap.setDevicePixelRatio(1);

    emit imageLoaded(_pixmap);
    update();
  }

public:
  FaviconOmniIconWidget(const QString &hostname, QWidget *parent) : OmniIconWidget(parent) {
    // qDebug() << "request favicon" << hostname;
    _requester = FaviconService::instance()->makeRequest(hostname, parent);
    connect(_requester, &AbstractFaviconRequest::finished, this, &FaviconOmniIconWidget::iconLoaded);
    connect(_requester, &AbstractFaviconRequest::failed, this, [this]() {
      qDebug() << "image loading failed";
      emit imageLoadingFailed();
    });
    _requester->start();
  }
};

class OmniIcon : public QWidget {
  Q_OBJECT

  OmniIconUrl _url;
  OmniIconUrl m_originalUrl;
  OmniIconWidget *_iconWidget = nullptr;
  QVBoxLayout *layout;
  bool m_failedToLoad = false;

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

  bool didFailToLoad() const { return m_failedToLoad; }

  void handleFailedLoading() {
    qWarning() << "Failed to load image" << _url.toString();
    if (auto fallback = _url.fallback()) {
      setUrl(*fallback, true);
    } else {
      setUrl(BuiltinOmniIconUrl("question-mark-circle"), true);
    }

    m_failedToLoad = true;
  }

  void setUrl(const OmniIconUrl &url, bool forceFallback = false) {
    if (!forceFallback && url == m_originalUrl) return;

    if (_iconWidget) {
      _iconWidget->blockSignals(true);
      layout->takeAt(0);
      _iconWidget->deleteLater();
    }

    _url = url;

    if (!forceFallback) {
      m_originalUrl = url;
      m_failedToLoad = false;
    }

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

  const OmniIconUrl &url() const { return _url; }

signals:
  void imageUpdated(const QPixmap &pixmap) const;
};
