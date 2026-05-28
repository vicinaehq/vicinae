#include "image-renderer.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-stream.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/contrast-helper.hpp"
#include "ui/image/url.hpp"
#include <QBuffer>
#include <QCoreApplication>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QMimeDatabase>
#include <QPainter>
#include <QPromise>
#include <QSvgRenderer>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent>
#include <QtMath>
#include <mutex>

namespace ImageRendering {

static constexpr int AA_PAD = 2;

QThreadPool &decodingPool() {
  static QThreadPool pool;
  static std::once_flag flag;
  std::call_once(flag, []() { pool.setMaxThreadCount(4); });
  return pool;
}

QImage renderBuiltinSvg(const QString &iconName, const QSize &size, const QColor &fg, const QColor &bg) {
  QString const iconPath = QStringLiteral(":icons/%1.svg").arg(iconName);
  QSvgRenderer renderer(iconPath);
  if (!renderer.isValid()) return {};

  bool const hasBg = bg.isValid() && bg.alpha() > 0;
  int const pad = hasBg ? 0 : AA_PAD;

  QImage canvas(size.width() + pad * 2, size.height() + pad * 2, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  QRectF const contentRect(pad, pad, size.width(), size.height());
  int margin = 0;

  if (hasBg) {
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

QImage renderEmoji(const QString &emoji, const QSize &size) {
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

QImage renderSystemIcon(const QString &name, const QSize &size) {
  // QIcon::fromTheme is not reentrant; serialize calls since this runs on the decoding pool.
  static std::mutex mtx;
  std::lock_guard lock(mtx);
  QIcon const icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};

  qreal const dpr = qGuiApp->devicePixelRatio();
  QSize const logicalSize(qCeil(size.width() / dpr), qCeil(size.height() / dpr));
  return icon.pixmap(logicalSize, dpr).toImage();
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

QImage renderFileIcon(const QString &path, const QSize &size, const QColor &fg, const QColor &bg) {
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
  QColor const builtinFg =
      fg.isValid() ? fg : ThemeService::instance().theme().resolve(SemanticColor::Foreground);
  return renderBuiltinSvg(builtinName, size, builtinFg, bg);
}

QImage decodeImageData(QIODevice *device, const QSize &size) {
  QImageReader reader(device);
  if (!reader.canRead()) return {};

  if (size.isValid()) {
    auto original = reader.size();
    if (original.isValid() && (original.width() > size.width() || original.height() > size.height()))
      reader.setScaledSize(original.scaled(size, Qt::KeepAspectRatio));
  }

  QImage result = reader.read();

  if (!result.isNull() && size.isValid() &&
      (result.width() > size.width() || result.height() > size.height())) {
    result = result.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  return result;
}

QImage decodeImageData(const QByteArray &data, const QSize &size) {
  QBuffer buf;
  buf.setData(data);
  buf.open(QIODevice::ReadOnly);
  return decodeImageData(&buf, size);
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

static void applyRoundedRectMask(QImage &image) {
  QImage masked(image.size(), QImage::Format_ARGB32_Premultiplied);
  masked.fill(Qt::transparent);
  QPainter painter(&masked);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(Qt::white);
  painter.setPen(Qt::NoPen);
  int const side = qMin(masked.width(), masked.height());
  qreal const radius = side * 0.25;
  painter.drawRoundedRect(masked.rect(), radius, radius);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.drawImage(0, 0, image);
  image = masked;
}

void applySafetyMargins(QImage &image) {
  if (image.isNull()) return;
  int const side = qMin(image.width(), image.height());
  int const margin = qRound(side * 0.07);

  QImage padded(image.size(), QImage::Format_ARGB32_Premultiplied);
  padded.fill(Qt::transparent);
  QRectF const dest(margin, margin, image.width() - margin * 2, image.height() - margin * 2);
  QPainter painter(&padded);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawImage(dest, image);
  image = padded;
}

void applyPostTransforms(QImage &image, const QColor &fg, OmniPainter::ImageMaskType mask) {
  if (image.isNull()) return;
  if (fg.isValid()) applyFillColor(image, fg);
  switch (mask) {
  case OmniPainter::CircleMask:
    applyCircleMask(image);
    break;
  case OmniPainter::RoundedRectangleMask:
    applyRoundedRectMask(image);
    break;
  case OmniPainter::NoMask:
    break;
  }
}

QFuture<QImage> renderFavicon(const QString &domain, const QSize &size, const QColor &fg,
                              OmniPainter::ImageMaskType mask) {
  auto promise = std::make_shared<QPromise<QImage>>();
  auto future = promise->future();
  promise->start();

  QMetaObject::invokeMethod(
      qApp,
      [promise, domain, size, fg, mask]() {
        auto *svc = FaviconService::instance();
        if (!svc) {
          promise->addResult(QImage{});
          promise->finish();
          return;
        }
        auto faviconFuture = svc->makeRequest(domain);
        auto *watcher = new QFutureWatcher<FaviconService::FaviconResponse>;
        QObject::connect(watcher, &QFutureWatcherBase::finished, qApp, [promise, watcher, size, fg, mask]() {
          auto result = watcher->result();
          if (result) {
            QImage img = result.value().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation).toImage();
            applyPostTransforms(img, fg, mask);
            promise->addResult(std::move(img));
          } else {
            promise->addResult(QImage{});
          }
          promise->finish();
          watcher->deleteLater();
        });
        watcher->setFuture(faviconFuture);
      },
      Qt::QueuedConnection);

  return future;
}

QImage decodeAndTransform(const QByteArray &data, const QSize &size, const QColor &fg,
                          OmniPainter::ImageMaskType mask) {
  QImage img;

  if (data.trimmed().startsWith("<?xml") || data.trimmed().startsWith("<svg")) {
    QSvgRenderer renderer(data);
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
    img = decodeImageData(data, size);
  }

  applyPostTransforms(img, fg, mask);
  return img;
}

QFuture<QImage> renderFirstFrame(const ImageURL &url, const QSize &size, bool safetyMargins) {
  auto promise = std::make_shared<QPromise<QImage>>();
  auto future = promise->future();
  promise->start();

  QMetaObject::invokeMethod(
      qApp,
      [promise, url, size, safetyMargins]() {
        auto *stream = new ImageStream(url, size, {.safetyMargins = safetyMargins});
        auto finalize = [promise, stream](QImage img) {
          if (!promise->future().isFinished()) {
            promise->addResult(std::move(img));
            promise->finish();
          }
          stream->deleteLater();
        };
        QObject::connect(stream, &ImageStream::frameReady, stream,
                         [finalize](const QImage &img) { finalize(img); });
        QObject::connect(stream, &ImageStream::failed, stream, [finalize]() { finalize(QImage{}); });
        stream->start();
      },
      Qt::QueuedConnection);

  return future;
}

QThread &animationThread() {
  static auto *thread = [] {
    auto *t = new QThread();
    t->setObjectName(QStringLiteral("GifAnimation"));
    t->start();
    return t;
  }();
  return *thread;
}

} // namespace ImageRendering
