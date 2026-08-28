#include "image-stream.hpp"
#include "data-uri/data-uri.hpp"
#include "image-fetcher.hpp"
#include "image-renderer.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#ifdef Q_OS_MACOS
#include "ui/image/mac-file-icon-loader.hpp"
#endif
#ifdef Q_OS_WIN
#include "ui/image/win-file-icon-loader.hpp"
#endif
#include <QBuffer>
#include <QCache>
#include <QFutureWatcher>
#include <QIcon>
#include <QImageReader>
#include <QMovie>
#include <QThread>
#include <QtConcurrent>
#include <variant>

class AnimFrameWorker : public QObject {
  Q_OBJECT
public:
  using QObject::QObject;
signals:
  void frameReady(const QImage &frame);
};

// Worker result: either a fully decoded static frame, or the source bytes for an animation.
using DecodeResult = std::variant<QImage, QByteArray>;

static QCache<QString, QImage> &imageCache() {
  static QCache<QString, QImage> cache(64 * 1024 * 1024);
  return cache;
}

static QCache<QString, QImage> &latestImageCache() {
  static QCache<QString, QImage> cache(256);
  return cache;
}

static QCache<QString, QByteArray> &bytesCache() {
  static QCache<QString, QByteArray> cache(32 * 1024 * 1024);
  return cache;
}

namespace ImageRendering {
void clearCache() {
  imageCache().clear();
  latestImageCache().clear();
  bytesCache().clear();
}
} // namespace ImageRendering

static QString makeLatestCacheKey(const ImageURL &url) {
  auto key = url.toString();
  if (url.type() == ImageURLType::System || url.type() == ImageURLType::FileIcon)
    key += QStringLiteral("|it:") + QIcon::themeName();
  return key;
}

static QString makeCacheKey(const ImageURL &url, const QSize &size, bool safetyMargins) {
  auto key = QStringLiteral("%1|%2x%3").arg(url.toString()).arg(size.width()).arg(size.height());
  if (safetyMargins) key += QStringLiteral("|m");
  if (url.type() == ImageURLType::System || url.type() == ImageURLType::FileIcon)
    key += QStringLiteral("|it:") + QIcon::themeName();
  return key;
}

namespace ImageRendering {
std::optional<QImage> cachedFrame(const ImageURL &url) {
  if (auto *cached = latestImageCache().object(makeLatestCacheKey(url.resolved()))) return *cached;
  return std::nullopt;
}
} // namespace ImageRendering

static bool isGif(const QByteArray &data) {
  return data.size() >= 6 && (data.startsWith("GIF87a") || data.startsWith("GIF89a"));
}

static bool isMultiFrameGif(const QByteArray &data) {
  if (!isGif(data)) return false;
  QBuffer buf;
  buf.setData(data);
  buf.open(QIODevice::ReadOnly);
  QImageReader reader(&buf);
  return reader.imageCount() > 1;
}

static QColor resolveBackgroundTint(const ImageURL &url) {
  if (auto bg = url.backgroundTint()) {
    QColor const c = ThemeService::instance().theme().resolve(*bg);
    if (c.isValid() && c.alpha() > 0) return c;
  }
  return {};
}

ImageStream::ImageStream(const ImageURL &url, const QSize &size, ImageStreamOptions opts, QObject *parent)
    : QObject(parent), m_url(url.resolved()), m_size(size), m_opts(opts) {
  if (auto fill = m_url.fillColor()) m_fg = ThemeService::instance().theme().resolve(*fill);
  m_bg = resolveBackgroundTint(m_url);
  m_mask = m_url.mask();
  m_cacheKey = makeCacheKey(m_url, size, m_opts.safetyMargins);
  m_originalCacheKey = m_cacheKey;
  m_latestCacheKey = makeLatestCacheKey(m_url);
  m_originalLatestCacheKey = m_latestCacheKey;
}

ImageStream::~ImageStream() {
  m_canceled->store(true, std::memory_order_relaxed);
  if (m_movie) m_movie->deleteLater();
  if (m_pendingReply) {
    m_pendingReply->abort();
    m_pendingReply->deleteLater();
  }
}

bool ImageStream::start() {
  if (m_opts.cache) {
    if (auto *cached = imageCache().object(m_cacheKey)) {
      emit frameReady(*cached);
      return true;
    }
  }
  dispatch();
  return false;
}

void ImageStream::dispatch() {
  switch (m_url.type()) {
  case ImageURLType::Http:
  case ImageURLType::Https:
  case ImageURLType::DataURI:
  case ImageURLType::Local:
    startFetchable();
    break;
  default:
    startStatic();
    break;
  }
}

void ImageStream::tryFallback() {
  if (m_fallbacksRemaining <= 0) {
    emit failed();
    return;
  }
  m_fallbacksRemaining--;

  if (auto fb = m_url.fallback())
    m_url = ImageURL(*fb).resolved();
  else
    m_url = ImageURL::builtin(BuiltinIcon::QuestionMarkCircle).resolved();

  m_fg = QColor();
  if (auto fill = m_url.fillColor()) m_fg = ThemeService::instance().theme().resolve(*fill);
  m_bg = resolveBackgroundTint(m_url);
  m_mask = m_url.mask();
  m_cacheKey = makeCacheKey(m_url, m_size, m_opts.safetyMargins);
  m_latestCacheKey = makeLatestCacheKey(m_url);

  if (m_opts.cache) {
    if (auto *cached = imageCache().object(m_cacheKey)) {
      emit frameReady(*cached);
      return;
    }
  }
  dispatch();
}

void ImageStream::handleStaticFuture(QFuture<QImage> future) {
  auto *watcher = new QFutureWatcher<QImage>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    watcher->deleteLater();
    emitStaticFrame(watcher->result());
  });
  watcher->setFuture(future);
}

void ImageStream::startStatic() {
  const QString name = m_url.name();
  // With a backdrop, content renders at the inset size and the post stage
  // composes it onto the full-size tile.
  const QSize renderSize = m_bg.isValid() ? ImageRendering::backdropContentSize(m_size) : m_size;

  auto canceled = m_canceled;
  auto runInPool = [this, canceled](auto renderFn, const QColor &postFg) {
    handleStaticFuture(QtConcurrent::run(&ImageRendering::decodingPool(),
                                         [renderFn = std::move(renderFn), postFg, bg = m_bg, size = m_size,
                                          mask = m_mask, canceled]() -> QImage {
                                           if (canceled->load(std::memory_order_relaxed)) return {};
                                           QImage img = renderFn();
                                           ImageRendering::applyPostTransforms(img, postFg, bg, size, mask);
                                           return img;
                                         }));
  };

  switch (m_url.type()) {
  case ImageURLType::Builtin:
    runInPool([name, size = renderSize]() { return ImageRendering::renderBuiltinSvg(name, size); }, m_fg);
    break;
  case ImageURLType::Emoji:
    runInPool([name, size = renderSize]() { return ImageRendering::renderEmoji(name, size); }, m_fg);
    break;
  case ImageURLType::Symbol:
    runInPool([name, size = renderSize]() { return ImageRendering::renderSymbol(name, size); }, m_fg);
    break;
  case ImageURLType::FontPreview:
    runInPool([name, size = renderSize]() { return ImageRendering::renderFontPreview(name, size); }, m_fg);
    break;
  case ImageURLType::System:
    runInPool([name, size = renderSize]() { return ImageRendering::renderSystemIcon(name, size); }, m_fg);
    break;
#ifdef Q_OS_MACOS
  case ImageURLType::MacBundle:
    runInPool([name, size = renderSize]() { return renderMacFileIcon(name, size); }, m_fg);
    break;
#endif
#ifdef Q_OS_WIN
  case ImageURLType::WinShellIcon:
    runInPool([name, size = renderSize]() { return renderWinShellIcon(name, size); }, m_fg);
    break;
  case ImageURLType::WinStockIcon:
    runInPool([name, size = renderSize]() { return renderWinStockIcon(name.toInt(), size); }, m_fg);
    break;
#endif
  case ImageURLType::FileIcon:
    runInPool(
        [name, size = renderSize, fg = m_fg]() { return ImageRendering::renderFileIcon(name, size, fg); },
        QColor());
    break;
  case ImageURLType::Favicon:
    handleStaticFuture(ImageRendering::renderFavicon(name, m_size, m_fg, m_bg, m_mask));
    break;
  default:
    tryFallback();
    break;
  }
}

void ImageStream::startFetchable() {
  auto type = m_url.type();
  const QString &name = m_url.name();

  if (m_opts.cache) {
    if (auto *bytes = bytesCache().object(name)) {
      onDataReceived(*bytes);
      return;
    }
  }

  if (type == ImageURLType::Local) {
    auto canceled = m_canceled;
    auto future = QtConcurrent::run(
        &ImageRendering::decodingPool(),
        [name, canceled, size = m_size, fg = m_fg, bg = m_bg, mask = m_mask]() -> DecodeResult {
          if (canceled->load(std::memory_order_relaxed)) return QImage{};
          QFile f(name);
          if (!f.open(QIODevice::ReadOnly)) return QImage{};

          // Animated GIFs need full bytes for QMovie; everything else streams.
          if (isGif(f.peek(6))) {
            QByteArray data = f.readAll();
            if (isMultiFrameGif(data)) return data;
            return ImageRendering::decodeAndTransform(data, size, fg, bg, mask);
          }

          // SVG can't be read by QImageReader; fall back to the bytes path.
          if (name.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive))
            return ImageRendering::decodeAndTransform(f.readAll(), size, fg, bg, mask);

          // Raster: stream-decode directly from the file (no intermediate QByteArray).
          QSize const decodeSize = bg.isValid() ? ImageRendering::backdropContentSize(size) : size;
          QImage img = ImageRendering::decodeImageData(&f, decodeSize);
          ImageRendering::applyPostTransforms(img, fg, bg, size, mask);
          return img;
        });
    auto *watcher = new QFutureWatcher<DecodeResult>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
      auto result = watcher->result();
      watcher->deleteLater();
      if (auto *animData = std::get_if<QByteArray>(&result)) {
        if (m_opts.cache && !bytesCache().contains(m_url.name()))
          bytesCache().insert(m_url.name(), new QByteArray(*animData), animData->size());
        startAnimation(std::move(*animData));
        return;
      }
      auto &img = std::get<QImage>(result);
      if (img.isNull()) {
        tryFallback();
        return;
      }
      emitStaticFrame(std::move(img));
    });
    watcher->setFuture(future);
    return;
  }

  if (type == ImageURLType::DataURI) {
    DataUri const uri(name);
    QByteArray decoded = uri.decodeContent();
    if (decoded.isEmpty()) {
      tryFallback();
      return;
    }
    bool const isSvg = uri.mediaType().contains(QStringLiteral("svg"));
    if (isSvg) {
      decodeStatic(decoded);
      return;
    }
    onDataReceived(decoded);
    return;
  }

  QString rawUrl = name;
  if (rawUrl.startsWith(QStringLiteral("https:/")) && !rawUrl.startsWith(QStringLiteral("https://")))
    rawUrl.insert(6, '/');
  else if (rawUrl.startsWith(QStringLiteral("http:/")) && !rawUrl.startsWith(QStringLiteral("http://")))
    rawUrl.insert(5, '/');

  m_pendingReply = NetworkFetcher::instance()->fetch(QUrl(rawUrl));
  connect(m_pendingReply, &FetchReply::finished, this, [this](const QByteArray &data) {
    m_pendingReply->deleteLater();
    m_pendingReply = nullptr;
    onDataReceived(data);
  });
}

void ImageStream::onDataReceived(const QByteArray &data) {
  if (data.isEmpty()) {
    tryFallback();
    return;
  }

  if (m_opts.cache && !bytesCache().contains(m_url.name()))
    bytesCache().insert(m_url.name(), new QByteArray(data), data.size());

  auto canceled = m_canceled;
  auto future = QtConcurrent::run(
      &ImageRendering::decodingPool(),
      [data, size = m_size, fg = m_fg, bg = m_bg, mask = m_mask, canceled]() -> DecodeResult {
        if (canceled->load(std::memory_order_relaxed)) return QImage{};
        if (isMultiFrameGif(data)) return data;
        return ImageRendering::decodeAndTransform(data, size, fg, bg, mask);
      });
  auto *watcher = new QFutureWatcher<DecodeResult>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    auto result = watcher->result();
    watcher->deleteLater();
    if (auto *animData = std::get_if<QByteArray>(&result)) {
      startAnimation(std::move(*animData));
      return;
    }
    emitStaticFrame(std::move(std::get<QImage>(result)));
  });
  watcher->setFuture(future);
}

void ImageStream::decodeStatic(const QByteArray &data) {
  auto canceled = m_canceled;
  auto future =
      QtConcurrent::run(&ImageRendering::decodingPool(),
                        [data, size = m_size, fg = m_fg, bg = m_bg, mask = m_mask, canceled]() -> QImage {
                          if (canceled->load(std::memory_order_relaxed)) return {};
                          return ImageRendering::decodeAndTransform(data, size, fg, bg, mask);
                        });
  auto *watcher = new QFutureWatcher<QImage>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    watcher->deleteLater();
    emitStaticFrame(watcher->result());
  });
  watcher->setFuture(future);
}

void ImageStream::emitStaticFrame(QImage img) {
  if (img.isNull()) {
    tryFallback();
    return;
  }
  if (m_opts.safetyMargins && m_url.type() != ImageURLType::MacBundle)
    ImageRendering::applySafetyMargins(img);
  if (m_opts.cache) {
    auto cost = static_cast<int>(img.sizeInBytes());
    imageCache().insert(m_cacheKey, new QImage(img), cost);
    if (m_originalCacheKey != m_cacheKey) imageCache().insert(m_originalCacheKey, new QImage(img), cost);
    latestImageCache().insert(m_latestCacheKey, new QImage(img));
    if (m_originalLatestCacheKey != m_latestCacheKey)
      latestImageCache().insert(m_originalLatestCacheKey, new QImage(img));
  }
  emit frameReady(img);
}

void ImageStream::startAnimation(QByteArray data) {
  auto *buffer = new QBuffer();
  buffer->setData(data);
  buffer->open(QIODevice::ReadOnly);

  auto *movie = new QMovie();
  movie->setDevice(buffer);
  buffer->setParent(movie);

  if (!movie->isValid() || movie->frameCount() <= 1) {
    delete movie;
    decodeStatic(data);
    return;
  }

  QSize const native = movie->currentPixmap().size();
  if (!native.isEmpty() && (native.width() > m_size.width() || native.height() > m_size.height()))
    movie->setScaledSize(native.scaled(m_size, Qt::KeepAspectRatio));

  auto *worker = new AnimFrameWorker(movie);
  auto mask = m_mask;
  auto canceled = m_canceled;
  auto safetyMargins = m_opts.safetyMargins;

  connect(movie, &QMovie::updated, worker, [movie, worker, mask, canceled, safetyMargins]() {
    if (canceled->load(std::memory_order_relaxed)) {
      movie->stop();
      return;
    }
    QImage frame = movie->currentImage();
    if (frame.isNull()) return;
    ImageRendering::applyPostTransforms(frame, QColor(), QColor(), QSize(), mask);
    if (safetyMargins) ImageRendering::applySafetyMargins(frame);
    emit worker->frameReady(frame);
  });

  connect(worker, &AnimFrameWorker::frameReady, this, [this](const QImage &frame) {
    if (m_opts.cache) {
      latestImageCache().insert(m_latestCacheKey, new QImage(frame));
      if (m_originalLatestCacheKey != m_latestCacheKey)
        latestImageCache().insert(m_originalLatestCacheKey, new QImage(frame));
    }
    emit this->frameReady(frame);
  });

  m_movie = movie;
  movie->moveToThread(&ImageRendering::animationThread());
  QMetaObject::invokeMethod(movie, &QMovie::start, Qt::QueuedConnection);
}

#include "image-stream.moc"
