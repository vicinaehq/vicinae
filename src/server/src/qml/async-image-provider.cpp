#include "async-image-provider.hpp"
#include "builtin_icon.hpp"
#include "image-data-cache.hpp"
#include "image-fetcher.hpp"
#include "ui/image/image-renderer.hpp"
#include "ui/image/url.hpp"
#include "data-uri/data-uri.hpp"
#include <QCoreApplication>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QPainter>
#include <QQuickTextureFactory>
#include <QSvgRenderer>
#include <QTimer>
#include <QUrlQuery>
#include <QtConcurrent>
#include <QtMath>
#include <atomic>

static QString remapLegacyKey(const QString &key) {
  if (key == QStringLiteral("fg")) return QStringLiteral("fill");
  if (key == QStringLiteral("bg")) return QStringLiteral("bg_tint");
  return key;
}

static ImageURL parseProviderId(const QString &id) {
  if (id.startsWith(QStringLiteral("icon://"))) return ImageURL(id);

  int colonIdx = id.indexOf(':');
  if (colonIdx < 0) return {};

  QString typeStr = id.left(colonIdx);
  QString name = id.mid(colonIdx + 1);
  QUrlQuery query;

  int semiIdx = typeStr.indexOf(';');
  if (semiIdx >= 0) {
    for (const auto &p : typeStr.mid(semiIdx + 1).split(';')) {
      int eq = p.indexOf('=');
      if (eq < 0) continue;
      query.addQueryItem(remapLegacyKey(p.left(eq)), QUrl::fromPercentEncoding(p.mid(eq + 1).toUtf8()));
    }
    typeStr = typeStr.left(semiIdx);
  }

  bool isBlob = (typeStr == QStringLiteral("http") || typeStr == QStringLiteral("datauri"));
  if (!isBlob) {
    int qmark = name.indexOf('?');
    if (qmark >= 0) {
      for (const auto &p : name.mid(qmark + 1).split('&')) {
        int eq = p.indexOf('=');
        if (eq < 0) continue;
        query.addQueryItem(remapLegacyKey(p.left(eq)), QUrl::fromPercentEncoding(p.mid(eq + 1).toUtf8()));
      }
      name = name.left(qmark);
    }
  }

  QUrl iconUrl;
  iconUrl.setScheme(QStringLiteral("icon"));
  iconUrl.setHost(typeStr);
  iconUrl.setPath(QStringLiteral("/") + name);
  if (!query.isEmpty()) iconUrl.setQuery(query);

  return ImageURL(iconUrl);
}

static bool isGif(const QByteArray &data) {
  return data.size() >= 6 && (data.startsWith("GIF87a") || data.startsWith("GIF89a"));
}

class ViciImageResponse;
static void dispatchRenderToResponse(ViciImageResponse *response, const ImageURL &url, const QSize &size);

class ViciImageResponse : public QQuickImageResponse {
  Q_OBJECT

public:
  explicit ViciImageResponse(qreal dpr = 1.0) : m_dpr(dpr) {}

  void setFallbackUrl(const ImageURL &fallback, const QSize &size) {
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
    if (image.isNull() && tryDispatchFallback()) return;
    image.setDevicePixelRatio(m_dpr);
    m_image = std::move(image);
    emit finished();
  }

private:
  bool tryDispatchFallback() {
    if (!m_fallback.isValid()) return false;
    ImageURL fb = std::move(m_fallback);
    m_fallback = {};
    QMetaObject::invokeMethod(
        this, [this, fb = std::move(fb)]() { dispatchRenderToResponse(this, fb, m_size); },
        Qt::QueuedConnection);
    return true;
  }

  qreal m_dpr;
  QImage m_image;
  QSize m_size;
  ImageURL m_fallback;
  std::atomic<bool> m_cancelled{false};
};

static void dispatchRenderToResponse(ViciImageResponse *response, const ImageURL &url, const QSize &size) {
  auto future = ImageRendering::render(url, size);
  auto *watcher = new QFutureWatcher<QImage>(response);
  QObject::connect(watcher, &QFutureWatcherBase::finished, response, [response, watcher]() {
    response->finish(watcher->result());
    watcher->deleteLater();
  });
  watcher->setFuture(future);
}

static void dispatchHttpWithGifDetection(ViciImageResponse *response, const ImageURL &url, const QSize &size,
                                         const QString &cacheId) {
  QColor fg;
  if (auto fill = url.fillColor()) fg = OmniPainter::resolveColor(*fill);
  auto mask = url.mask();

  QString rawUrl = url.name();
  if (rawUrl.startsWith(QStringLiteral("https:/")) && !rawUrl.startsWith(QStringLiteral("https://")))
    rawUrl.insert(6, '/');
  else if (rawUrl.startsWith(QStringLiteral("http:/")) && !rawUrl.startsWith(QStringLiteral("http://")))
    rawUrl.insert(5, '/');

  QMetaObject::invokeMethod(
      qApp,
      [response, rawUrl, size, mask, fg, cacheId]() {
        if (response->isCancelled()) {
          response->finish({});
          return;
        }
        auto *reply = NetworkFetcher::instance()->fetch(QUrl(rawUrl));
        QObject::connect(reply, &FetchReply::finished, qApp,
                         [response, reply, size, mask, fg, cacheId](const QByteArray &data) {
                           reply->deleteLater();
                           if (response->isCancelled()) {
                             response->finish({});
                             return;
                           }
                           if (isGif(data))
                             ImageDataCache::instance().storeAnimated(cacheId, data);
                           else
                             ImageDataCache::instance().storeNotAnimated(cacheId);
                           QtConcurrent::run(&ImageRendering::decodingPool(),
                                             [response, data, size, fg, mask]() {
                                               QImage img = ImageRendering::decodeImageData(data, size);
                                               ImageRendering::applyPostTransforms(img, fg, mask);
                                               response->finish(std::move(img));
                                             });
                         });
      },
      Qt::QueuedConnection);
}

static void dispatchDataUriWithGifDetection(ViciImageResponse *response, const ImageURL &url,
                                            const QSize &size, const QString &cacheId) {
  QColor fg;
  if (auto fill = url.fillColor()) fg = OmniPainter::resolveColor(*fill);
  auto mask = url.mask();
  QString const dataStr = url.name();

  QtConcurrent::run(&ImageRendering::decodingPool(), [response, dataStr, size, fg, mask, cacheId]() {
    DataUri const uri(dataStr);
    QByteArray const decoded = uri.decodeContent();
    if (decoded.isEmpty()) {
      response->finish({});
      return;
    }

    bool const isSvg = uri.mediaType().contains(QStringLiteral("svg"));
    if (!isSvg) {
      if (isGif(decoded))
        ImageDataCache::instance().storeAnimated(cacheId, decoded);
      else
        ImageDataCache::instance().storeNotAnimated(cacheId);
    }

    QImage img;
    if (isSvg) {
      QSvgRenderer renderer(decoded);
      if (renderer.isValid()) {
        img = QImage(size, QImage::Format_ARGB32_Premultiplied);
        img.fill(Qt::transparent);
        QPainter painter(&img);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        renderer.setAspectRatioMode(Qt::KeepAspectRatio);
        renderer.render(&painter, img.rect());
      }
    } else {
      img = ImageRendering::decodeImageData(decoded, size);
    }

    ImageRendering::applyPostTransforms(img, fg, mask);
    response->finish(std::move(img));
  });
}

QQuickImageResponse *AsyncImageProvider::requestImageResponse(const QString &id, const QSize &requestedSize) {
  qreal const dpr = qGuiApp->devicePixelRatio();
  auto *response = new ViciImageResponse(dpr);
  QSize const logical = requestedSize.isValid() && !requestedSize.isEmpty() ? requestedSize : QSize(64, 64);
  QSize const size(qCeil(logical.width() * dpr), qCeil(logical.height() * dpr));

  auto url = parseProviderId(id);

  auto fallbackStr = url.fallback();
  ImageURL fallbackUrl =
      fallbackStr ? parseProviderId(*fallbackStr) : ImageURL::builtin(BuiltinIconService::unknownIcon());

  if (fallbackUrl.isValid()) response->setFallbackUrl(fallbackUrl, size);

  if (!url.isValid()) {
    QTimer::singleShot(0, response, [response]() { response->finish({}); });
    return response;
  }

  switch (url.type()) {
  case ImageURLType::Http:
  case ImageURLType::Https:
    dispatchHttpWithGifDetection(response, url, size, id);
    break;
  case ImageURLType::DataURI:
    dispatchDataUriWithGifDetection(response, url, size, id);
    break;
  default:
    dispatchRenderToResponse(response, url, size);
    break;
  }

  return response;
}

#include "async-image-provider.moc"
