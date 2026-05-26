#include "async-image-provider.hpp"
#include "builtin_icon.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-fetcher.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/contrast-helper.hpp"
#ifdef Q_OS_MACOS
#include "ui/image/mac-bundle-icon-loader.hpp"
#endif
#include <QCoreApplication>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QQuickTextureFactory>
#include <QMimeDatabase>
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

static bool isGif(const QByteArray &data) {
  return data.size() >= 6 && (data.startsWith("GIF87a") || data.startsWith("GIF89a"));
}

static QThreadPool &imageDecodingPool() {
  static QThreadPool pool;
  static std::once_flag flag;
  std::call_once(flag, []() { pool.setMaxThreadCount(1); });
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
    image.setDevicePixelRatio(m_dpr);
    m_image = std::move(image);
    QTimer::singleShot(0, this, &QQuickImageResponse::finished);
  }

private:
  void tryFallback(QImage &image);

  QString m_id;
  qreal m_dpr;
  QImage m_image;
  QSize m_size;
  QString m_fallback;
  std::atomic<bool> m_cancelled{false};
};

static constexpr int kAAPad = 2;

static QImage renderBuiltinSvg(const QString &iconName, const QSize &size, const QColor &fg,
                               const QColor &bg) {
  QString const iconPath = QStringLiteral(":icons/%1.svg").arg(iconName);
  QSvgRenderer renderer(iconPath);
  if (!renderer.isValid()) return {};

  QImage canvas(size.width() + kAAPad * 2, size.height() + kAAPad * 2, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  QRectF const contentRect(kAAPad, kAAPad, size.width(), size.height());
  int margin = 0;

  if (bg.isValid() && bg.alpha() > 0) {
    int const side = qMin(size.width(), size.height());
    qreal const radius = side * 0.25;
    margin = qRound(side * 0.19);
    painter.setBrush(bg);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(contentRect, radius, radius);
  }

  QRectF const iconRect =
      contentRect.marginsRemoved({qreal(margin), qreal(margin), qreal(margin), qreal(margin)});

  QImage svgImage(iconRect.size().toSize(), QImage::Format_ARGB32_Premultiplied);
  svgImage.fill(Qt::transparent);
  {
    QPainter svgPainter(&svgImage);
    svgPainter.setRenderHint(QPainter::Antialiasing, true);
    svgPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    renderer.setAspectRatioMode(Qt::KeepAspectRatio);
    renderer.render(&svgPainter, svgImage.rect());

    QColor fillColor = fg;
    if (bg.isValid() && bg.alpha() > 0) fillColor = ContrastHelper::getTonalContrastColor(bg, 5);

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
  QIcon const icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};
  return icon.pixmap(size).toImage();
}

static QImage renderLocalSvg(const QString &path, const QSize &size) {
  QSvgRenderer renderer(path);
  if (!renderer.isValid()) return {};

  // Respect the SVG's natural size; don't upscale beyond it.
  // This keeps small SVGs (e.g. 45x45 viewBox) at their intended
  // visual size rather than stretching them to fill the cell.
  QSize renderSize = size;
  QSize const natural = renderer.defaultSize();
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
    QSize const original = reader.size();
    if (original.isValid() && (original.width() > size.width() || original.height() > size.height()))
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatio));
  }

  QImage img = reader.read();
  if (img.isNull()) return {};

  if (size.isValid() && (img.width() > size.width() || img.height() > size.height()))
    img = img.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QImage padded(img.width() + kAAPad * 2, img.height() + kAAPad * 2, QImage::Format_ARGB32_Premultiplied);
  padded.fill(Qt::transparent);
  QPainter painter(&padded);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawImage(kAAPad, kAAPad, img);
  return padded;
}

static QImage decodeImageData(const QByteArray &data, const QSize &size) {
  QBuffer buf;
  buf.setData(data);
  buf.open(QIODevice::ReadOnly);

  QImageReader reader(&buf);
  if (!reader.canRead()) return {};

  if (size.isValid()) {
    auto original = reader.size();
    if (original.isValid() && (original.width() > size.width() || original.height() > size.height()))
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatioByExpanding));
  }

  QImage result = reader.read();

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

static void applyPostTransforms(QImage &image, const QColor &fg, bool circleMask) {
  if (image.isNull()) return;
  if (fg.isValid()) applyFillColor(image, fg);
  if (circleMask) applyCircleMask(image);
}

template <typename Render>
static void dispatchRender(ViciImageResponse *response, QColor fg, bool circleMask, Render render) {
  imageDecodingPool().start([response, fg, circleMask, render = std::move(render)]() {
    if (response->isCancelled()) {
      response->finish({});
      return;
    }
    QImage img = render();
    applyPostTransforms(img, fg, circleMask);
    response->finish(std::move(img));
  });
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
    int const eq = param.indexOf('=');
    if (eq < 0) continue;
    QString const key = param.left(eq);
    QString const val = QUrl::fromPercentEncoding(param.mid(eq + 1).toUtf8());
    if (key == QStringLiteral("fg"))
      result.fg = QColor(val);
    else if (key == QStringLiteral("bg"))
      result.bg = QColor(val);
    else if (key == QStringLiteral("mask") && val == QStringLiteral("circle"))
      result.circleMask = true;
    else if (key == QStringLiteral("fallback"))
      result.fallback = val;
  }
}

static ParsedId parseId(const QString &id) {
  ParsedId result;

  int const colonIdx = id.indexOf(':');
  if (colonIdx < 0) return result;

  QString const typeStr = id.left(colonIdx);
  QString const rest = id.mid(colonIdx + 1);

  int const semiIdx = typeStr.indexOf(';');
  if (semiIdx >= 0) {
    result.type = typeStr.left(semiIdx);
    parseParams(typeStr.mid(semiIdx + 1), ';', result);
  } else {
    result.type = typeStr;
  }

  if (result.type == QStringLiteral("http") || result.type == QStringLiteral("datauri")) {
    result.name = rest;
  } else {
    int const qmark = rest.indexOf('?');
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

  QString const fallback = m_fallback;
  m_fallback.clear();
  auto fb = parseId(fallback);

  if (fb.type == QStringLiteral("builtin")) {
    QColor const fg =
        fb.fg.isValid() ? fb.fg : ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    image = renderBuiltinSvg(fb.name, m_size, fg, fb.bg);
    applyPostTransforms(image, QColor(), fb.circleMask);
  } else if (fb.type == QStringLiteral("system")) {
    image = renderSystemIcon(fb.name, m_size);
    applyPostTransforms(image, fb.fg, fb.circleMask);
  }
}

QQuickImageResponse *AsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {

  qreal const dpr = qGuiApp->devicePixelRatio();
  auto *response = new ViciImageResponse(dpr);
  response->setId(id);
  QSize const logical = requestedSize.isValid() && !requestedSize.isEmpty() ? requestedSize : QSize(64, 64);
  QSize const size(qCeil(logical.width() * dpr), qCeil(logical.height() * dpr));
  auto parsed = parseId(id);

  static const QString DEFAULT_FALLBACK =
      QStringLiteral("builtin:") + BuiltinIconService::nameForIcon(BuiltinIconService::unknownIcon());

  if (!parsed.fallback.isEmpty())
    response->setFallback(parsed.fallback, size);
  else if (parsed.type != QStringLiteral("builtin"))
    response->setFallback(DEFAULT_FALLBACK, size);

  QColor const fg = parsed.fg;
  bool const circle = parsed.circleMask;

  if (parsed.type == QStringLiteral("builtin")) {
    QColor builtinFg = fg;
    if (!builtinFg.isValid()) builtinFg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    QColor const bg = parsed.bg;
    dispatchRender(response, QColor(), circle, [name = parsed.name, size, builtinFg, bg]() {
      return renderBuiltinSvg(name, size, builtinFg, bg);
    });

  } else if (parsed.type == QStringLiteral("emoji")) {
    dispatchRender(response, fg, circle, [name = parsed.name, size]() { return renderEmoji(name, size); });

  } else if (parsed.type == QStringLiteral("system")) {
    dispatchRender(response, fg, circle,
                   [name = parsed.name, size]() { return renderSystemIcon(name, size); });

#ifdef Q_OS_MACOS
  } else if (parsed.type == QStringLiteral("bundle")) {
    dispatchRender(response, fg, circle,
                   [path = parsed.name, size]() { return renderMacBundleIcon(path, size); });
#endif

  } else if (parsed.type == QStringLiteral("local")) {
    QString const path = parsed.name;
    if (path.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive)) {
      dispatchRender(response, fg, circle, [path, size]() { return renderLocalSvg(path, size); });
    } else {
      dispatchRender(response, fg, circle, [path, size]() { return renderLocalRaster(path, size); });
    }

  } else if (parsed.type == QStringLiteral("file-icon")) {
    QString const path = parsed.name;
    QColor const defaultFg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    QColor const bg = parsed.bg;
    dispatchRender(response, QColor(), circle, [path, size, fg, defaultFg, bg]() {
      QMimeDatabase const db;
      auto const mime = db.mimeTypeForFile(path, QMimeDatabase::MatchDefault);

      QImage img = renderSystemIcon(mime.iconName(), size);
      if (img.isNull()) img = renderSystemIcon(mime.genericIconName(), size);

      if (!img.isNull()) {
        if (fg.isValid()) applyFillColor(img, fg);
        return img;
      }

      QString const builtinName = mime.name() == QStringLiteral("inode/directory")
                                      ? QStringLiteral("folder")
                                      : QStringLiteral("blank-document");
      return renderBuiltinSvg(builtinName, size, fg.isValid() ? fg : defaultFg, bg);
    });

  } else if (parsed.type == QStringLiteral("http")) {
    QString url = parsed.name;
    if (url.startsWith(QStringLiteral("https:/")) && !url.startsWith(QStringLiteral("https://")))
      url.insert(6, '/');
    else if (url.startsWith(QStringLiteral("http:/")) && !url.startsWith(QStringLiteral("http://")))
      url.insert(5, '/');
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
                             dispatchRender(response, fg, circle,
                                            [data, size]() { return decodeImageData(data, size); });
                           });
        },
        Qt::QueuedConnection);

  } else if (parsed.type == QStringLiteral("favicon")) {
    QMetaObject::invokeMethod(
        qApp,
        [response, domain = parsed.name, size, fg, circle]() {
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
          // No parent: watcher must live on main thread, response lives on reader thread
          auto *watcher = new QFutureWatcher<FaviconService::FaviconResponse>;
          QObject::connect(
              watcher, &QFutureWatcherBase::finished, qApp, [response, watcher, size, fg, circle]() {
                auto result = watcher->result();
                if (!response->isCancelled() && result) {
                  QImage img =
                      result.value().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
                  applyPostTransforms(img, fg, circle);
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
    QString const dataStr = parsed.name;
    dispatchRender(response, fg, circle, [dataStr, size, id]() {
      DataUri const uri(dataStr);
      QByteArray const decoded = uri.decodeContent();
      if (decoded.isEmpty()) return QImage();

      bool const isSvg = uri.mediaType().contains(QStringLiteral("svg"));
      if (!isSvg) {
        if (isGif(decoded))
          ImageDataCache::instance().storeAnimated(id, decoded);
        else
          ImageDataCache::instance().storeNotAnimated(id);
      }

      if (!isSvg) return decodeImageData(decoded, size);

      QSvgRenderer renderer(decoded);
      if (!renderer.isValid()) return QImage();
      QImage img(size, QImage::Format_ARGB32_Premultiplied);
      img.fill(Qt::transparent);
      QPainter painter(&img);
      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
      renderer.setAspectRatioMode(Qt::KeepAspectRatio);
      renderer.render(&painter, img.rect());
      return img;
    });

  } else {
    response->finishDeferred({});
  }

  return response;
}

#include "async-image-provider.moc"
