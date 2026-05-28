#include "image-stream.hpp"
#include "data-uri/data-uri.hpp"
#include "image-fetcher.hpp"
#include "image-renderer.hpp"
#include <QBuffer>
#include <QCache>
#include <QFutureWatcher>
#include <QImageReader>
#include <QMovie>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>

class AnimFrameWorker : public QObject {
  Q_OBJECT
public:
  using QObject::QObject;
signals:
  void frameReady(const QImage &frame);
};

static QCache<QString, QImage> &imageCache() {
  static QCache<QString, QImage> cache(64 * 1024 * 1024);
  return cache;
}

static QString makeCacheKey(const ImageURL &url, const QSize &size) {
  return QStringLiteral("%1|%2x%3").arg(url.cacheKey()).arg(size.width()).arg(size.height());
}

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

ImageStream::ImageStream(const ImageURL &url, const QSize &size, QObject *parent)
    : QObject(parent), m_url(url), m_size(size) {
  if (auto fill = url.fillColor()) m_fg = OmniPainter::resolveColor(*fill);
  m_mask = url.mask();
  m_cacheKey = makeCacheKey(url, size);

  if (auto *cached = imageCache().object(m_cacheKey)) {
    QTimer::singleShot(0, this, [this, img = *cached]() { emit frameReady(img); });
    return;
  }
  switch (url.type()) {
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

std::optional<QImage> ImageStream::findCached(const ImageURL &url, const QSize &size) {
  if (auto *img = imageCache().object(makeCacheKey(url, size))) return *img;
  return std::nullopt;
}

ImageStream::~ImageStream() {
  m_canceled->store(true, std::memory_order_relaxed);
  if (m_movie) m_movie->deleteLater();
  if (m_pendingReply) {
    m_pendingReply->abort();
    m_pendingReply->deleteLater();
  }
}

void ImageStream::startStatic() {
  auto future = ImageRendering::render(m_url, m_size);
  auto *watcher = new QFutureWatcher<QImage>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    watcher->deleteLater();
    emitStaticFrame(watcher->result());
  });
  watcher->setFuture(future);
}

void ImageStream::startFetchable() {
  auto type = m_url.type();
  const QString &name = m_url.name();

  if (type == ImageURLType::Local) {
    auto canceled = m_canceled;
    auto future = QtConcurrent::run(&ImageRendering::decodingPool(), [name, canceled]() -> QByteArray {
      if (canceled->load(std::memory_order_relaxed)) return {};
      QFile f(name);
      if (f.open(QIODevice::ReadOnly)) return f.readAll();
      return {};
    });
    auto *watcher = new QFutureWatcher<QByteArray>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
      watcher->deleteLater();
      onDataReceived(watcher->result());
    });
    watcher->setFuture(future);
    return;
  }

  if (type == ImageURLType::DataURI) {
    DataUri const uri(name);
    QByteArray decoded = uri.decodeContent();
    if (decoded.isEmpty()) {
      emit failed();
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
    emit failed();
    return;
  }

  auto canceled = m_canceled;
  auto future = QtConcurrent::run(
      &ImageRendering::decodingPool(),
      [data, size = m_size, fg = m_fg, mask = m_mask, canceled]() -> std::pair<QImage, QByteArray> {
        if (canceled->load(std::memory_order_relaxed)) return {};
        if (isMultiFrameGif(data)) return {{}, data};
        return {ImageRendering::decodeAndTransform(data, size, fg, mask), {}};
      });
  auto *watcher = new QFutureWatcher<std::pair<QImage, QByteArray>>(this);
  connect(watcher, &QFutureWatcherBase::finished, this, [this, watcher]() {
    auto [img, animData] = watcher->result();
    watcher->deleteLater();
    if (!animData.isEmpty()) {
      startAnimation(std::move(animData));
      return;
    }
    emitStaticFrame(std::move(img));
  });
  watcher->setFuture(future);
}

void ImageStream::decodeStatic(const QByteArray &data) {
  auto canceled = m_canceled;
  auto future = QtConcurrent::run(&ImageRendering::decodingPool(),
                                  [data, size = m_size, fg = m_fg, mask = m_mask, canceled]() -> QImage {
                                    if (canceled->load(std::memory_order_relaxed)) return {};
                                    return ImageRendering::decodeAndTransform(data, size, fg, mask);
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
    emit failed();
    return;
  }
  auto cost = static_cast<int>(img.sizeInBytes());
  imageCache().insert(m_cacheKey, new QImage(img), cost);
  emit frameReady(img);
}

void ImageStream::startAnimation(QByteArray data) {
  auto *buffer = new QBuffer();
  buffer->setData(data);
  buffer->open(QIODevice::ReadOnly);

  auto *movie = new QMovie();
  movie->setDevice(buffer);
  buffer->setParent(movie);
  movie->setCacheMode(QMovie::CacheAll);

  if (!movie->isValid() || movie->frameCount() <= 1) {
    delete movie;
    decodeStatic(data);
    return;
  }

  QSize const native = movie->currentPixmap().size();
  if (!native.isEmpty() && (native.width() > m_size.width() || native.height() > m_size.height()))
    movie->setScaledSize(native.scaled(m_size, Qt::KeepAspectRatio));

  auto *worker = new AnimFrameWorker(movie);
  auto fg = m_fg;
  auto mask = m_mask;
  auto canceled = m_canceled;

  connect(movie, &QMovie::updated, worker, [movie, worker, fg, mask, canceled]() {
    if (canceled->load(std::memory_order_relaxed)) {
      movie->stop();
      return;
    }
    QImage frame = movie->currentImage();
    if (frame.isNull()) return;
    ImageRendering::applyPostTransforms(frame, fg, mask);
    emit worker->frameReady(frame);
  });

  connect(worker, &AnimFrameWorker::frameReady, this,
          [this](const QImage &frame) { emit this->frameReady(frame); });

  m_movie = movie;
  movie->moveToThread(&ImageRendering::animationThread());
  QMetaObject::invokeMethod(movie, &QMovie::start, Qt::QueuedConnection);
}

#include "image-stream.moc"
