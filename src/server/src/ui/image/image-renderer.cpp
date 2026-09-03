#include "image-renderer.hpp"
#include "favicon/favicon-service.hpp"
#include "font-service.hpp"
#include "image-stream.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/theme-file.hpp"
#include "ui/image/contrast-helper.hpp"
#include "ui/image/url.hpp"
#ifdef Q_OS_MACOS
#include "ui/image/mac-file-icon-loader.hpp"
#endif
#ifdef Q_OS_WIN
#include "ui/image/win-file-icon-loader.hpp"
#endif
#include <QBuffer>
#include <algorithm>
#include <array>
#include <cmath>
#include <QCoreApplication>
#include <QFontMetricsF>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QMimeDatabase>
#include <QPainter>
#include <QPromise>
#include <QRawFont>
#include <QSvgRenderer>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent>
#include <QtMath>
#include <algorithm>
#include <mutex>

namespace ImageRendering {

static constexpr int AA_PAD = 2;

QThreadPool &decodingPool() {
  static QThreadPool pool;
  static std::once_flag flag;
  std::call_once(flag, []() { pool.setMaxThreadCount(4); });
  return pool;
}

QImage renderBuiltinSvg(const QString &iconName, const QSize &size) {
  QString const iconPath = QStringLiteral(":icons/%1.svg").arg(iconName);
  QSvgRenderer renderer(iconPath);
  if (!renderer.isValid()) return {};

  QImage canvas(size.width() + AA_PAD * 2, size.height() + AA_PAD * 2, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  renderer.setAspectRatioMode(Qt::KeepAspectRatio);
  renderer.render(&painter, QRectF(AA_PAD, AA_PAD, size.width(), size.height()));
  return canvas;
}

// Shared glyph draw. Emoji and symbols differ only in the font and whether font
// merging (fallback) is allowed: emoji force the color emoji font with no
// merging; symbols use a text font with merging so fallback resolves the wide
// symbol ranges (math, box-drawing, etc.).
static QImage renderGlyph(const QString &glyph, const QSize &size, QFont font, bool allowMerging) {
  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);

  font.setStyleStrategy(allowMerging ? QFont::PreferDefault : QFont::NoFontMerging);
  font.setPixelSize(static_cast<int>(size.height() * 0.8));

  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setFont(font);
  painter.drawText(canvas.rect(), Qt::AlignCenter, glyph);
  return canvas;
}

static bool fontCoversEmoji(const QFont &font, const QString &emoji) {
  const QRawFont raw = QRawFont::fromFont(font);
  if (!raw.isValid()) return false;

  for (const char32_t cp : emoji.toStdU32String()) {
    // joiners, variation selectors and tags are not mapped by most fonts
    const bool invisible = cp == 0x200D || cp == 0xFE0E || cp == 0xFE0F || (cp >= 0xE0020 && cp <= 0xE007F);
    if (!invisible && !raw.supportsCharacter(static_cast<uint>(cp))) return false;
  }
  return true;
}

QImage renderEmoji(const QString &emoji, const QSize &size) {
  auto *fontService = ServiceRegistry::instance()->fontService();
  if (!fontService) return QImage(size, QImage::Format_ARGB32_Premultiplied);

  const QFont &font = fontService->emojiFont();
  return renderGlyph(emoji, size, font, /*allowMerging=*/!fontCoversEmoji(font, emoji));
}

QImage renderSymbol(const QString &symbol, const QSize &size) {
  auto *fontService = ServiceRegistry::instance()->fontService();
  if (!fontService) return QImage(size, QImage::Format_ARGB32_Premultiplied);
  QFont font;
  font.setFamilies(fontService->symbolFontFamilies());
  return renderGlyph(symbol, size, font, /*allowMerging=*/true);
}

// spec is "family<US>glyph" (see ImageURL::fontPreview). Scales the glyph's ink box to fit
// the canvas so glyphs of any size are uniformly sized and never clipped.
QImage renderFontPreview(const QString &spec, const QSize &size) {
  const qsizetype sep = spec.indexOf(QChar(0x1F));
  const QString family = sep >= 0 ? spec.left(sep) : spec;
  const QString glyph = sep >= 0 ? spec.sliced(sep + 1) : QStringLiteral("Aa");

  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);

  // Empty family => placeholder: default font with merging so the glyph always resolves.
  QFont font = family.isEmpty() ? QFont() : QFont(family);
  font.setStyleStrategy(family.isEmpty() ? QFont::PreferDefault : QFont::NoFontMerging);
  font.setPixelSize(size.height());

  const QFontMetricsF metrics(font);
  const QRectF ink = metrics.tightBoundingRect(glyph);
  if (ink.isEmpty()) return canvas;

  constexpr qreal FILL = 0.7;
  const qreal scale = std::min(size.width() * FILL / ink.width(), size.height() * FILL / ink.height());

  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.setFont(font);
  painter.translate(size.width() / 2.0, size.height() / 2.0);
  painter.scale(scale, scale);
  painter.drawText(QPointF(-ink.center().x(), -ink.center().y()), glyph);
  return canvas;
}

QImage renderSystemIcon(const QString &name, const QSize &size) {
#ifdef Q_OS_MACOS
  return renderMacSymbolIcon(name, size);
#endif

  // QIconLoader and QPixmap are GUI-thread only.
  // Calling it from another thread can result in the icon not being found from the very beginning or after
  // the next cache update.
  // https://github.com/vicinaehq/vicinae/issues/1450
  if (QThread::currentThread() != qApp->thread()) {
    QImage result;
    QMetaObject::invokeMethod(
        qApp, [&] { result = renderSystemIcon(name, size); }, Qt::BlockingQueuedConnection);
    return result;
  }

  QIcon const icon = QIcon::fromTheme(name);
  if (icon.isNull()) return {};

  qreal const dpr = qGuiApp->devicePixelRatio();
  QSize const logicalSize(qCeil(size.width() / dpr), qCeil(size.height() / dpr));
  return icon.pixmap(logicalSize, dpr).toImage();
}

// QPainter scales sources by their device pixel ratio; paint through a DPR-neutral copy so
// high-DPR renders keep their pixel size.
static void applyFillColor(QImage &image, const QColor &fg) {
  if (!fg.isValid()) return;
  QImage source = image;
  source.setDevicePixelRatio(1.0);
  QImage tinted(image.size(), QImage::Format_ARGB32_Premultiplied);
  tinted.fill(Qt::transparent);
  QPainter painter(&tinted);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawImage(0, 0, source);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.fillRect(tinted.rect(), fg);
  painter.end();
  tinted.setDevicePixelRatio(image.devicePixelRatio());
  image = tinted;
}

// Luminance becomes the mask: on a light fill, bright pixels are ink; on a dark fill,
// dark pixels are. Contrast is stretched between the 3rd and 97th luminance percentiles and
// pushed through an S-curve so a glyph on a solid disc still reads and anti-aliased edges
// don't smear into halos. Single-tone glyphs have nothing to stretch and get the silhouette.
static void applyTemplateFill(QImage &image, const QColor &fg) {
  if (!fg.isValid() || image.isNull()) return;

  constexpr float MIN_TONAL_RANGE = 0.12F;
  constexpr int MIN_SAMPLE_ALPHA = 128;
  constexpr int MIN_OUT_ALPHA = 10;
  constexpr std::size_t BINS = 64;
  constexpr float LOW_PERCENTILE = 0.03F;
  constexpr float HIGH_PERCENTILE = 0.97F;
  constexpr float CURVE_LOW = 0.18F;
  constexpr float CURVE_HIGH = 0.82F;

  QImage source = image.convertToFormat(QImage::Format_ARGB32);
  auto luminance = [](QRgb px) {
    return (0.2126F * qRed(px) + 0.7152F * qGreen(px) + 0.0722F * qBlue(px)) / 255.0F;
  };

  std::array<std::size_t, BINS> histogram{};
  std::size_t samples = 0;
  for (int y = 0; y < source.height(); ++y) {
    const auto *row = reinterpret_cast<const QRgb *>(source.constScanLine(y));
    for (int x = 0; x < source.width(); ++x) {
      if (qAlpha(row[x]) < MIN_SAMPLE_ALPHA) continue;
      const auto bin = std::min(BINS - 1, static_cast<std::size_t>(luminance(row[x]) * BINS));
      histogram[bin]++;
      samples++;
    }
  }
  if (samples == 0) return;

  auto percentile = [&](float p) {
    const auto target = static_cast<std::size_t>(p * static_cast<float>(samples));
    std::size_t seen = 0;
    for (std::size_t i = 0; i < BINS; ++i) {
      seen += histogram[i];
      if (seen > target) return (static_cast<float>(i) + 0.5F) / BINS;
    }
    return 1.0F;
  };
  const float lo = percentile(LOW_PERCENTILE);
  const float hi = percentile(HIGH_PERCENTILE);

  if (hi - lo < MIN_TONAL_RANGE) {
    applyFillColor(image, fg);
    return;
  }

  const bool lightInk = fg.lightnessF() > 0.5F;
  const float range = hi - lo;
  auto curve = [](float t) {
    t = std::clamp((t - CURVE_LOW) / (CURVE_HIGH - CURVE_LOW), 0.0F, 1.0F);
    return t * t * (3.0F - 2.0F * t);
  };

  QImage out(source.size(), QImage::Format_ARGB32);
  for (int y = 0; y < source.height(); ++y) {
    const auto *in = reinterpret_cast<const QRgb *>(source.constScanLine(y));
    auto *dst = reinterpret_cast<QRgb *>(out.scanLine(y));
    for (int x = 0; x < source.width(); ++x) {
      const int alpha = qAlpha(in[x]);
      if (alpha == 0) {
        dst[x] = 0;
        continue;
      }
      float ink = std::clamp((luminance(in[x]) - lo) / range, 0.0F, 1.0F);
      if (!lightInk) ink = 1.0F - ink;
      ink = curve(ink);
      int outAlpha = static_cast<int>(std::lround(alpha * ink * fg.alphaF()));
      if (outAlpha < MIN_OUT_ALPHA) outAlpha = 0;
      dst[x] = qRgba(fg.red(), fg.green(), fg.blue(), std::clamp(outAlpha, 0, 255));
    }
  }
  out.setDevicePixelRatio(image.devicePixelRatio());
  image = out.convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

QImage renderFileIcon(const QString &path, const QSize &size, const QColor &fg) {
#ifdef Q_OS_MACOS
  if (!fg.isValid()) {
    if (QImage native = renderMacFileIcon(path, size); !native.isNull()) { return native; }
  }
#endif
#ifdef Q_OS_WIN
  if (!fg.isValid()) {
    if (QImage native = renderWinShellIcon(path, size); !native.isNull()) { return native; }
  }
#endif

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
  QImage builtin = renderBuiltinSvg(builtinName, size);
  applyFillColor(builtin, builtinFg);
  return builtin;
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

static bool hasBackdrop(const QColor &bg) { return bg.isValid() && bg.alpha() > 0; }

QSize backdropContentSize(const QSize &size) {
  int const margin = qRound(qMin(size.width(), size.height()) * 0.19);
  return {std::max(1, size.width() - margin * 2), std::max(1, size.height() - margin * 2)};
}

// Theme tints are tuned for text on the app background, not as fills: their
// lightness varies per hue, which makes a row of tiles look uneven. Clamp
// every tile into one tonal band so the set reads as a family and always
// supports a light glyph.
static QColor clampTileTone(const QColor &bg) {
  float h, s, l, a;
  bg.getHslF(&h, &s, &l, &a);
  if (h < 0.f) h = 0.f;
  if (s > 0.05f) s = std::clamp(s, 0.55f, 0.8f);
  l = std::clamp(l, 0.42f, 0.52f);
  return QColor::fromHslF(h, s, l, a);
}

static void applyBackdrop(QImage &image, const QSize &size, const QColor &tile) {
  QImage canvas(size, QImage::Format_ARGB32_Premultiplied);
  canvas.fill(Qt::transparent);
  QPainter painter(&canvas);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

  QRectF const tileRect(0, 0, size.width(), size.height());
  int const side = qMin(size.width(), size.height());
  qreal const radius = side * 0.25;

  auto shifted = [](const QColor &c, float dh, float ds, float dl) {
    float h, s, l, a;
    c.getHslF(&h, &s, &l, &a);
    if (h < 0.f) h = 0.f;
    h = std::fmod(h + dh + 1.f, 1.f);
    s = std::clamp(s + ds, 0.f, 1.f);
    l = std::clamp(l + dl, 0.f, 1.f);
    return QColor::fromHslF(h, s, l, a);
  };

  QLinearGradient gradient(tileRect.topLeft(), tileRect.bottomLeft());
  gradient.setColorAt(0, shifted(tile, 0.025f, -0.03f, 0.10f));
  gradient.setColorAt(1, shifted(tile, -0.015f, 0.06f, -0.05f));
  painter.setBrush(gradient);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(tileRect, radius, radius);

  qreal const strokeWidth = std::max(1.0, side / 32.0);
  painter.setBrush(Qt::NoBrush);
  painter.setPen(QPen(QColor(255, 255, 255, 30), strokeWidth));
  qreal const inset = strokeWidth / 2.0;
  painter.drawRoundedRect(tileRect.adjusted(inset, inset, -inset, -inset), radius - inset, radius - inset);

  QRectF const dest((size.width() - image.width()) / 2.0, (size.height() - image.height()) / 2.0,
                    image.width(), image.height());

  QImage shadow(image.size(), QImage::Format_ARGB32_Premultiplied);
  shadow.fill(Qt::transparent);
  {
    QPainter shadowPainter(&shadow);
    shadowPainter.drawImage(0, 0, image);
    shadowPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    shadowPainter.fillRect(shadow.rect(), QColor(0, 0, 0, 70));
  }
  painter.drawImage(dest.translated(0, side * 0.035), shadow);
  painter.drawImage(dest, image);
  image = canvas;
}

// Icons pad their glyph very differently; trimming to the visible bounds and re-centering in a
// square with a uniform margin gives a row of template icons one optical size.
static void normalizeTemplateBounds(QImage &image) {
  constexpr int VISIBLE_ALPHA = 24;
  constexpr float MARGIN = 0.08F;

  QImage src = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  src.setDevicePixelRatio(1.0);
  int left = src.width();
  int top = src.height();
  int right = -1;
  int bottom = -1;
  for (int y = 0; y < src.height(); ++y) {
    const auto *row = reinterpret_cast<const QRgb *>(src.constScanLine(y));
    for (int x = 0; x < src.width(); ++x) {
      if (qAlpha(row[x]) < VISIBLE_ALPHA) continue;
      left = std::min(left, x);
      right = std::max(right, x);
      top = std::min(top, y);
      bottom = std::max(bottom, y);
    }
  }
  if (right < left || bottom < top) return;

  QRect const bounds(left, top, right - left + 1, bottom - top + 1);
  int const side = std::max(bounds.width(), bounds.height());
  int const margin = static_cast<int>(std::lround(side * MARGIN));
  int const canvas = side + 2 * margin;

  QImage out(canvas, canvas, QImage::Format_ARGB32_Premultiplied);
  out.fill(Qt::transparent);
  QPainter painter(&out);
  painter.drawImage(QPoint(margin + (side - bounds.width()) / 2, margin + (side - bounds.height()) / 2), src,
                    bounds);
  painter.end();
  out.setDevicePixelRatio(image.devicePixelRatio());
  image = out;
}

QSize templateRenderSize(const QSize &size) {
  constexpr int SUPERSAMPLE = 4;
  constexpr int MAX_SIDE = 512;
  if (!size.isValid()) return size;
  QSize out = size * SUPERSAMPLE;
  if (out.width() > MAX_SIDE || out.height() > MAX_SIDE)
    out = out.scaled(MAX_SIDE, MAX_SIDE, Qt::KeepAspectRatio);
  return out;
}

void applyPostTransforms(QImage &image, const QColor &fg, const QColor &bg, const QSize &size,
                         OmniPainter::ImageMaskType mask, bool templateFill) {
  if (image.isNull()) return;
  bool const hasBg = hasBackdrop(bg) && size.isValid();
  QColor const tile = hasBg ? clampTileTone(bg) : QColor();

  // On a tile, tintable content gets a contrast-derived fill instead of the
  // requested one; untinted content (emoji, raster) keeps its own colors.
  QColor fill = fg;
  if (hasBg && fg.isValid()) fill = ContrastHelper::getTonalContrastColor(tile, 5, 0.1);
  if (fill.isValid()) {
    if (templateFill) {
      applyTemplateFill(image, fill);
      normalizeTemplateBounds(image);
      QSize const target = hasBg ? backdropContentSize(size) : size;
      if (target.isValid() && image.size() != target) {
        image = image.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
      }
    } else {
      applyFillColor(image, fill);
    }
  }
  if (hasBg) applyBackdrop(image, size, tile);

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

QFuture<QImage> renderFavicon(const QString &domain, const QSize &size, const QColor &fg, const QColor &bg,
                              OmniPainter::ImageMaskType mask) {
  auto promise = std::make_shared<QPromise<QImage>>();
  auto future = promise->future();
  promise->start();

  QMetaObject::invokeMethod(
      qApp,
      [promise, domain, size, fg, bg, mask]() {
        auto *svc = FaviconService::instance();
        if (!svc) {
          promise->addResult(QImage{});
          promise->finish();
          return;
        }
        auto faviconFuture = svc->makeRequest(domain);
        auto *watcher = new QFutureWatcher<FaviconService::FaviconResponse>;
        QObject::connect(
            watcher, &QFutureWatcherBase::finished, qApp, [promise, watcher, size, fg, bg, mask]() {
              auto result = watcher->result();
              if (result) {
                QSize const contentSize = hasBackdrop(bg) ? backdropContentSize(size) : size;
                QImage img = result.value()
                                 .scaled(contentSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                                 .toImage();
                applyPostTransforms(img, fg, bg, size, mask);
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

QImage decodeAndTransform(const QByteArray &data, const QSize &size, const QColor &fg, const QColor &bg,
                          OmniPainter::ImageMaskType mask, bool templateFill) {
  QSize contentSize = hasBackdrop(bg) ? backdropContentSize(size) : size;
  if (templateFill) contentSize = templateRenderSize(contentSize);
  QImage img;

  if (data.trimmed().startsWith("<?xml") || data.trimmed().startsWith("<svg")) {
    QSvgRenderer renderer(data);
    if (renderer.isValid()) {
      img = QImage(contentSize, QImage::Format_ARGB32_Premultiplied);
      img.fill(Qt::transparent);
      QPainter painter(&img);
      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
      renderer.setAspectRatioMode(Qt::KeepAspectRatio);
      renderer.render(&painter, img.rect());
    }
  } else {
    img = decodeImageData(data, contentSize);
  }

  applyPostTransforms(img, fg, bg, size, mask, templateFill);
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
