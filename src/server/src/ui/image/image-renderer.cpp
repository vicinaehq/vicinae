#include "image-renderer.hpp"
#include "builtin_icon.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-fetcher.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/contrast-helper.hpp"
#include "ui/image/url.hpp"
#ifdef Q_OS_MACOS
#include "ui/image/mac-bundle-icon-loader.hpp"
#endif
#include "data-uri/data-uri.hpp"
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
#include <QUrl>
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

static QImage renderBuiltinSvg(const QString &iconName, const QSize &size, const QColor &fg,
                               const QColor &bg) {
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
  static std::mutex mtx;
  std::lock_guard lock(mtx);
  QIcon const icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};
  return icon.pixmap(size).toImage();
}

static QImage renderLocalSvg(const QString &path, const QSize &size) {
  QSvgRenderer renderer(path);
  if (!renderer.isValid()) return {};

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

  QImage padded(img.width() + AA_PAD * 2, img.height() + AA_PAD * 2, QImage::Format_ARGB32_Premultiplied);
  padded.fill(Qt::transparent);
  QPainter painter(&padded);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawImage(AA_PAD, AA_PAD, img);
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

template <typename Fn>
static QFuture<QImage> dispatchToPool(QColor fg, OmniPainter::ImageMaskType mask, Fn render) {
  return QtConcurrent::run(&decodingPool(), [fg, mask, render = std::move(render)]() {
    QImage img = render();
    applyPostTransforms(img, fg, mask);
    return img;
  });
}

static QFuture<QImage> renderHttp(const QString &rawUrl, const QSize &size, QColor fg,
                                  OmniPainter::ImageMaskType mask) {
  auto promise = std::make_shared<QPromise<QImage>>();
  auto future = promise->future();
  promise->start();

  QString url = rawUrl;
  if (url.startsWith(QStringLiteral("https:/")) && !url.startsWith(QStringLiteral("https://")))
    url.insert(6, '/');
  else if (url.startsWith(QStringLiteral("http:/")) && !url.startsWith(QStringLiteral("http://")))
    url.insert(5, '/');

  QMetaObject::invokeMethod(
      qApp,
      [promise, url, size, fg, mask]() {
        auto *reply = NetworkFetcher::instance()->fetch(QUrl(url));
        QObject::connect(reply, &FetchReply::finished, qApp,
                         [promise, reply, size, fg, mask](const QByteArray &data) {
                           reply->deleteLater();
                           decodingPool().start([promise, data, size, fg, mask]() {
                             QImage img = decodeImageData(data, size);
                             applyPostTransforms(img, fg, mask);
                             promise->addResult(std::move(img));
                             promise->finish();
                           });
                         });
      },
      Qt::QueuedConnection);

  return future;
}

static QFuture<QImage> renderFavicon(const QString &domain, const QSize &size, QColor fg,
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

static QFuture<QImage> renderDataUri(const QString &dataStr, const QSize &size, QColor fg,
                                     OmniPainter::ImageMaskType mask) {
  return dispatchToPool(fg, mask, [dataStr, size]() {
    DataUri const uri(dataStr);
    QByteArray const decoded = uri.decodeContent();
    if (decoded.isEmpty()) return QImage();

    bool const isSvg = uri.mediaType().contains(QStringLiteral("svg"));
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
}

QFuture<QImage> render(const ImageURL &url, const QSize &size) {
  QColor fg;
  if (auto fill = url.fillColor()) fg = OmniPainter::resolveColor(*fill);

  QColor bg;
  if (auto bgTint = url.backgroundTint()) bg = OmniPainter::resolveColor(*bgTint);

  auto mask = url.mask();
  const QString &name = url.name();

  switch (url.type()) {
  case ImageURLType::Builtin: {
    QColor builtinFg =
        fg.isValid() ? fg : ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    return dispatchToPool(QColor(), mask, [name, size, builtinFg, bg]() {
      return renderBuiltinSvg(name, size, builtinFg, bg);
    });
  }

  case ImageURLType::Emoji:
    return dispatchToPool(fg, mask, [name, size]() { return renderEmoji(name, size); });

  case ImageURLType::System:
    return dispatchToPool(fg, mask, [name, size]() { return renderSystemIcon(name, size); });

#ifdef Q_OS_MACOS
  case ImageURLType::MacBundle:
    return dispatchToPool(fg, mask, [name, size]() { return renderMacBundleIcon(name, size); });
#endif

  case ImageURLType::Local:
    if (name.endsWith(QStringLiteral(".svg"), Qt::CaseInsensitive))
      return dispatchToPool(fg, mask, [name, size]() { return renderLocalSvg(name, size); });
    return dispatchToPool(fg, mask, [name, size]() { return renderLocalRaster(name, size); });

  case ImageURLType::FileIcon: {
    QColor const defaultFg = ThemeService::instance().theme().resolve(SemanticColor::Foreground);
    return dispatchToPool(QColor(), mask, [name, size, fg, defaultFg, bg]() {
      QMimeDatabase const db;
      auto const mime = db.mimeTypeForFile(name, QMimeDatabase::MatchDefault);

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
  }

  case ImageURLType::Http:
  case ImageURLType::Https:
    return renderHttp(name, size, fg, mask);

  case ImageURLType::Favicon:
    return renderFavicon(name, size, fg, mask);

  case ImageURLType::DataURI:
    return renderDataUri(name, size, fg, mask);

  default:
    return QtFuture::makeReadyValueFuture(QImage{});
  }
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

static QFuture<QImage> renderWithFallback(ImageURL url, const QSize &size, bool safetyMargins,
                                          int remaining) {
  return render(url, size)
      .then([url = std::move(url), size, safetyMargins, remaining](QImage img) -> QFuture<QImage> {
        if (!img.isNull()) {
          if (safetyMargins && url.type() != ImageURLType::MacBundle) applySafetyMargins(img);
          return QtFuture::makeReadyValueFuture(std::move(img));
        }
        if (remaining <= 0) return QtFuture::makeReadyValueFuture(QImage{});

        ImageURL next;
        if (auto fb = url.fallback())
          next = ImageURL(*fb);
        else
          next = ImageURL::builtin("question-mark-circle");

        return renderWithFallback(std::move(next), size, safetyMargins, remaining - 1);
      })
      .unwrap();
}

QFuture<QImage> renderFirstFrame(const ImageURL &url, const QSize &size, bool safetyMargins) {
  return renderWithFallback(url, size, safetyMargins, 2);
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
