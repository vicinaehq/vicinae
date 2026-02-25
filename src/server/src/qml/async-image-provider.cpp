#include "async-image-provider.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-fetcher.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/contrast-helper.hpp"
#include <QCoreApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QQuickTextureFactory>
#include <QSvgRenderer>
#include <QFutureWatcher>
#include <QtMath>
#include <QThreadPool>
#include <QTimer>
#include <QUrl>
#include "data-uri/data-uri.hpp"
#include "image-data-cache.hpp"
#include <atomic>
#include <mutex>
#include <set>

static bool isGif(const QByteArray &data) {
  return data.size() >= 6 && (data.startsWith("GIF87a") || data.startsWith("GIF89a"));
}

static std::mutex s_failedCacheMutex;
static std::set<QString> s_failedImageIds;

static QThreadPool &imageDecodingPool() {
  static QThreadPool pool;
  static std::once_flag flag;
  std::call_once(flag, []() { pool.setMaxThreadCount(2); });
  return pool;
}

class ViciImageResponse : public QQuickImageResponse {
  Q_OBJECT

public:
  explicit ViciImageResponse(qreal dpr = 1.0) : m_dpr(dpr) {}

  void setId(const QString &id) { m_id = id; }

  void setFallback(const QString &fallback, const QSize &size) {
    m_fallback = fallback;
    m_size = size;
  }

  QQuickTextureFactory *textureFactory() const override {
    if (m_image.isNull()) return nullptr;
    return QQuickTextureFactory::textureFactoryForImage(m_image);
  }

  QString errorString() const override {
    if (m_image.isNull() && !m_cancelled.load(std::memory_order_acquire))
      return QStringLiteral("Image load failed");
    return {};
  }

  void cancel() override { m_cancelled.store(true, std::memory_order_release); }

  bool isCancelled() const { return m_cancelled.load(std::memory_order_acquire); }

  void finish(QImage image) {
    if (m_cancelled.load(std::memory_order_acquire)) {
      emit finished();
      return;
    }
    tryFallback(image);
    cacheIfFailed(image);
    image.setDevicePixelRatio(m_dpr);
    m_image = std::move(image);
    emit finished();
  }

  // Schedule finish on next event-loop iteration (for sync producers
  // that complete immediately inside requestImageResponse).
  void finishDeferred(QImage image) {
    if (m_cancelled.load(std::memory_order_acquire)) {
      QTimer::singleShot(0, this, &QQuickImageResponse::finished);
      return;
    }
    tryFallback(image);
    cacheIfFailed(image);
    image.setDevicePixelRatio(m_dpr);
    m_image = std::move(image);
    QTimer::singleShot(0, this, &QQuickImageResponse::finished);
  }

private:
  void tryFallback(QImage &image);

  void cacheIfFailed(const QImage &image) {
    if (!image.isNull() || m_id.isEmpty()) return;
    std::lock_guard lock(s_failedCacheMutex);
    s_failedImageIds.insert(m_id);
  }

  QString m_id;
  qreal m_dpr;
  QImage m_image;
  QSize m_size;
  QString m_fallback;
  std::atomic<bool> m_cancelled{false};
};

static QImage renderBuiltinSvg(const QString &iconName, const QSize &size, const QColor &fg,
                               const QColor &bg) {
  QString iconPath = QStringLiteral(":icons/%1.svg").arg(iconName);
  QSvgRenderer renderer(iconPath);
  if (!renderer.isValid()) return {};

  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  int margin = 0;

  if (bg.isValid() && bg.alpha() > 0) {
    int side = qMin(size.width(), size.height());
    qreal radius = side * 0.25;
    margin = qRound(side * 0.15);
    painter.setBrush(bg);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(canvas.rect(), radius, radius);
  }

  QRect iconRect = canvas.rect().marginsRemoved({margin, margin, margin, margin});

  QImage svgImage(iconRect.size(), QImage::Format_ARGB32_Premultiplied);
  svgImage.fill(Qt::transparent);
  {
    QPainter svgPainter(&svgImage);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(&svgPainter, svgImage.rect());

    QColor fillColor = fg;
    if (bg.isValid() && bg.alpha() > 0) fillColor = ContrastHelper::getTonalContrastColor(bg, 3);

    svgPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    svgPainter.fillRect(svgImage.rect(), fillColor);
  }

  painter.drawImage(iconRect, svgImage);
  return canvas;
}

static QImage renderEmoji(const QString &emoji, const QSize &size) {
  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);

  auto *fontService = ServiceRegistry::instance()->fontService();
  if (!fontService) return canvas;

  QFont font = fontService->emojiFont();
  font.setStyleStrategy(QFont::NoFontMerging);
  font.setPixelSize(static_cast<int>(size.height() * 0.8));

  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setFont(font);
  painter.drawText(canvas.rect(), Qt::AlignCenter, emoji);
  return canvas;
}

static QImage renderSystemIcon(const QString &name, const QSize &size) {
  QIcon icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};
  return icon.pixmap(size).toImage();
}

static QImage renderLocalSvg(const QString &path, const QSize &size) {
  QSvgRenderer renderer(path);
  if (!renderer.isValid()) return {};

  // Respect the SVG's natural size — don't upscale beyond it.
  // This keeps small SVGs (e.g. 45x45 viewBox) at their intended
  // visual size rather than stretching them to fill the cell.
  QSize renderSize = size;
  QSize natural = renderer.defaultSize();
  if (natural.isValid() && !natural.isEmpty()) {
    renderSize.setWidth(qMin(size.width(), natural.width()));
    renderSize.setHeight(qMin(size.height(), natural.height()));
  }

  QImage canvas(renderSize, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  renderer.setAspectRatioMode(Qt::KeepAspectRatio);
  renderer.render(&painter, canvas.rect());
  return canvas;
}

static QImage renderLocalRaster(const QString &path, const QSize &size) {
  QImageReader reader(path);
  if (!reader.canRead()) return {};

  if (size.isValid()) {
    QSize original = reader.size();
    if (original.isValid() && (original.width() > size.width() || original.height() > size.height()))
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatioByExpanding));
  }

  QImage img = reader.read();

  // setScaledSize is silently ignored by most formats (PNG, WebP, …).
  // If the decoded image is still much larger than requested, scale it
  // down now so we don't keep a full-resolution bitmap alive.
  if (!img.isNull() && size.isValid() && (img.width() > size.width() || img.height() > size.height())) {
    img = img.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  }

  return img;
}

static QImage decodeImageData(const QByteArray &data, const QSize &size) {
  QImageReader reader;
  QBuffer *buf = new QBuffer;
  buf->setData(data);
  buf->open(QIODevice::ReadOnly);
  reader.setDevice(buf);

  if (!reader.canRead()) {
    delete buf;
    return {};
  }

  if (size.isValid()) {
    auto original = reader.size();
    if (original.isValid() && (original.width() > size.width() || original.height() > size.height()))
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatioByExpanding));
  }

  QImage result = reader.read();
  delete buf;

  // Scale down if the format ignored setScaledSize (see renderLocalRaster).
  if (!result.isNull() && size.isValid() &&
      (result.width() > size.width() || result.height() > size.height())) {
    result = result.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
  }

  return result;
}

static void applyFillColor(QImage &image, const QColor &fg) {
  if (!fg.isValid()) return;
  QImage tinted(image.size(), QImage::Format_ARGB32_Premultiplied);
  tinted.fill(Qt::transparent);
  QPainter painter(&tinted);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawImage(0, 0, image);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.fillRect(tinted.rect(), fg);
  image = tinted;
}

static void applyCircleMask(QImage &image) {
  QImage masked(image.size(), QImage::Format_ARGB32_Premultiplied);
  masked.fill(Qt::transparent);
  QPainter painter(&masked);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(Qt::white);
  painter.setPen(Qt::NoPen);
  painter.drawEllipse(masked.rect());
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.drawImage(0, 0, image);
  image = masked;
}

struct ParsedId {
  QString type;
  QString name;
  QColor fg;
  QColor bg;
  bool circleMask = false;
  QString fallback;
};

static void parseParams(const QString &str, QChar sep, ParsedId &result) {
  for (const auto &param : str.split(sep)) {
    int eq = param.indexOf('=');
    if (eq < 0) continue;
    QString key = param.left(eq);
    QString val = param.mid(eq + 1);
    if (key == QStringLiteral("fg"))
      result.fg = QColor(val);
    else if (key == QStringLiteral("bg"))
      result.bg = QColor(val);
    else if (key == QStringLiteral("mask") && val == QStringLiteral("circle"))
      result.circleMask = true;
    else if (key == QStringLiteral("fallback"))
      result.fallback = QUrl::fromPercentEncoding(val.toUtf8());
  }
}

static ParsedId parseId(const QString &id) {
  ParsedId result;

  int colonIdx = id.indexOf(':');
  if (colonIdx < 0) return result;

  QString typeStr = id.left(colonIdx);
  QString rest = id.mid(colonIdx + 1);

  int semiIdx = typeStr.indexOf(';');
  if (semiIdx >= 0) {
    result.type = typeStr.left(semiIdx);
    parseParams(typeStr.mid(semiIdx + 1), ';', result);
  } else {
    result.type = typeStr;
  }

  if (result.type == QStringLiteral("http") || result.type == QStringLiteral("datauri")) {
    result.name = rest;
  } else {
    int qmark = rest.indexOf('?');
    if (qmark < 0) {
      result.name = rest;
    } else {
      result.name = rest.left(qmark);
      parseParams(rest.mid(qmark + 1), '&', result);
    }
  }

  return result;
}

void ViciImageResponse::tryFallback(QImage &image) {
  if (!image.isNull() || m_fallback.isEmpty()) return;

  QString fallback = m_fallback;
  m_fallback.clear();
  auto fb = parseId(fallback);

  if (fb.type == QStringLiteral("builtin")) {
    QColor fg = fb.fg.isValid() ? fb.fg : ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    image = renderBuiltinSvg(fb.name, m_size, fg, fb.bg);
    if (fb.circleMask && !image.isNull()) applyCircleMask(image);
  } else if (fb.type == QStringLiteral("system")) {
    image = renderSystemIcon(fb.name, m_size);
    if (!image.isNull()) applyFillColor(image, fb.fg);
  }
}

QQuickImageResponse *AsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {

  {
    std::lock_guard lock(s_failedCacheMutex);
    if (s_failedImageIds.contains(id)) {
      auto *response = new ViciImageResponse(qGuiApp->devicePixelRatio());
      response->finishDeferred({});
      return response;
    }
  }

  qreal dpr = qGuiApp->devicePixelRatio();
  auto *response = new ViciImageResponse(dpr);
  response->setId(id);
  QSize logical = requestedSize.isValid() && !requestedSize.isEmpty() ? requestedSize : QSize(64, 64);
  QSize size(qCeil(logical.width() * dpr), qCeil(logical.height() * dpr));
  auto parsed = parseId(id);

  if (!parsed.fallback.isEmpty()) response->setFallback(parsed.fallback, size);

  if (parsed.type == QStringLiteral("builtin")) {
    QColor fg = parsed.fg;
    if (!fg.isValid()) fg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    QImage img = renderBuiltinSvg(parsed.name, size, fg, parsed.bg);
    if (parsed.circleMask && !img.isNull()) applyCircleMask(img);
    response->finishDeferred(std::move(img));

  } else if (parsed.type == QStringLiteral("emoji")) {
    QMetaObject::invokeMethod(
        qApp,
        [response, name = parsed.name, size]() {
          if (response->isCancelled()) {
            response->finish({});
            return;
          }
          QImage img = renderEmoji(name, size);
          response->finish(std::move(img));
        },
        Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("system")) {
    QColor fg = parsed.fg;
    QMetaObject::invokeMethod(
        qApp,
        [response, name = parsed.name, size, fg]() {
          if (response->isCancelled()) {
            response->finish({});
            return;
          }
          QImage img = renderSystemIcon(name, size);
          if (!img.isNull()) applyFillColor(img, fg);
          response->finish(std::move(img));
        },
        Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("local")) {
    QString path = parsed.name;
    bool circle = parsed.circleMask;
    QColor fg = parsed.fg;

    if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
      QImage img = renderLocalSvg(path, size);
      if (!img.isNull()) applyFillColor(img, fg);
      if (circle && !img.isNull()) applyCircleMask(img);
      response->finishDeferred(std::move(img));
    } else {
      imageDecodingPool().start([response, path, size, circle, fg]() {
        if (response->isCancelled()) {
          response->finish({});
          return;
        }
        QImage img = renderLocalRaster(path, size);
        if (!img.isNull()) applyFillColor(img, fg);
        if (circle && !img.isNull()) applyCircleMask(img);
        response->finish(std::move(img));
      });
    }

  } else if (parsed.type == QStringLiteral("http")) {
    QString url = parsed.name;
    if (url.startsWith(QStringLiteral("https:/")) && !url.startsWith(QStringLiteral("https://")))
      url.insert(6, '/');
    else if (url.startsWith(QStringLiteral("http:/")) && !url.startsWith(QStringLiteral("http://")))
      url.insert(5, '/');
    bool circle = parsed.circleMask;
    QColor fg = parsed.fg;
    QMetaObject::invokeMethod(
        qApp,
        [response, url, size, circle, fg, id]() {
          if (response->isCancelled()) {
            response->finish({});
            return;
          }
          auto *reply = NetworkFetcher::instance()->fetch(QUrl(url));
          QObject::connect(reply, &FetchReply::finished, qApp,
                           [response, reply, size, circle, fg, id](const QByteArray &data) {
                             reply->deleteLater();
                             if (response->isCancelled()) {
                               response->finish({});
                               return;
                             }
                             if (isGif(data))
                               ImageDataCache::instance().storeAnimated(id, data);
                             else
                               ImageDataCache::instance().storeNotAnimated(id);
                             imageDecodingPool().start([response, data, size, circle, fg]() {
                               if (response->isCancelled()) {
                                 response->finish({});
                                 return;
                               }
                               QImage img = decodeImageData(data, size);
                               if (!img.isNull()) applyFillColor(img, fg);
                               if (circle && !img.isNull()) applyCircleMask(img);
                               response->finish(std::move(img));
                             });
                           });
        },
        Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("favicon")) {
    QMetaObject::invokeMethod(
        qApp,
        [response, domain = parsed.name, size]() {
          if (response->isCancelled()) {
            response->finish({});
            return;
          }
          auto *svc = FaviconService::instance();
          if (!svc) {
            response->finish({});
            return;
          }
          auto future = svc->makeRequest(domain);
          // No parent — watcher must live on main thread, response lives on reader thread
          auto *watcher = new QFutureWatcher<FaviconService::FaviconResponse>;
          QObject::connect(watcher, &QFutureWatcherBase::finished, qApp, [response, watcher, size]() {
            auto result = watcher->result();
            if (!response->isCancelled() && result) {
              QImage img =
                  result.value().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
              response->finish(std::move(img));
            } else {
              response->finish({});
            }
            watcher->deleteLater();
          });
          watcher->setFuture(future);
        },
        Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("datauri")) {
    QString dataStr = parsed.name;
    bool circle = parsed.circleMask;
    QColor fg = parsed.fg;
    imageDecodingPool().start([response, dataStr, size, circle, fg, id]() {
      if (response->isCancelled()) {
        response->finish({});
        return;
      }

      DataUri uri(dataStr);
      QByteArray decoded = uri.decodeContent();
      if (decoded.isEmpty()) {
        response->finish({});
        return;
      }

      bool isSvg = uri.mediaType().contains(QStringLiteral("svg"));

      if (!isSvg) {
        if (isGif(decoded))
          ImageDataCache::instance().storeAnimated(id, decoded);
        else
          ImageDataCache::instance().storeNotAnimated(id);
      }

      QImage img;
      if (isSvg) {
        QSvgRenderer renderer(decoded);
        if (renderer.isValid()) {
          img = QImage(size, QImage::Format_ARGB32_Premultiplied);
          img.fill(Qt::transparent);
          QPainter painter(&img);
          renderer.setAspectRatioMode(Qt::KeepAspectRatio);
          renderer.render(&painter, img.rect());
        }
      } else {
        img = decodeImageData(decoded, size);
      }

      if (!img.isNull()) applyFillColor(img, fg);
      if (circle && !img.isNull()) applyCircleMask(img);
      response->finish(std::move(img));
    });

  } else {
    response->finishDeferred({});
  }

  return response;
}

#include "async-image-provider.moc"
