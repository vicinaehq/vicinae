#include <QBuffer>
#include <QCache>
#include <QCryptographicHash>
#include <QHash>
#include <QImage>
#include <QPainter>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <jkqtmathtext/jkqtmathtext.h>
#include <cmath>
#include <cstddef>
#include <mutex>
#include "math-renderer.hpp"

namespace {
constexpr qreal FONT_SIZE = 32;
constexpr int CACHE_BYTES = 16 * 1024 * 1024;
constexpr int RASTER_CACHE_KIB = 32 * 1024;
constexpr int MAX_IMAGE_PIXELS = 16 * 1024 * 1024;
std::mutex resourceMutex;
QHash<QUrl, std::weak_ptr<const math::Resource>> resources;
std::mutex cacheMutex;
QCache<QString, std::shared_ptr<const math::Resource>> cache(CACHE_BYTES);

struct RasterKey {
  QUrl url;
  QSizeF size;
  qreal devicePixelRatio;

  bool operator==(const RasterKey &) const = default;
  friend std::size_t qHash(const RasterKey &key, std::size_t seed = 0) {
    return qHashMulti(seed, key.url, key.size.width(), key.size.height(), key.devicePixelRatio);
  }
};

std::mutex rasterCacheMutex;
QCache<RasterKey, QImage> rasterCache(RASTER_CACHE_KIB);
} // namespace

math::Resource::~Resource() {
  const std::lock_guard lock(resourceMutex);
  const auto it = resources.find(QUrl(url));
  if (it != resources.end() && it->expired()) resources.erase(it);
}

std::shared_ptr<const math::Resource> math::resource(const QUrl &url) {
  const std::lock_guard lock(resourceMutex);
  const auto it = resources.constFind(url);
  return it != resources.cend() ? it->lock() : nullptr;
}

std::shared_ptr<const math::Resource> math::render(const QString &latex, bool display, const QColor &color) {
  if (latex.size() > 8192) return {};
  const QString key = color.name(QColor::HexArgb) + (display ? "D" : "I") + latex;
  {
    const std::lock_guard lock(cacheMutex);
    if (const auto *value = cache.object(key)) return *value;
  }

  const auto url = QStringLiteral("vicinae-math:/%1")
                       .arg(QString::fromLatin1(
                           QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha256).toHex()));
  if (const auto existing = resource(QUrl(url))) return existing;
  JKQTMathText expression;
  expression.setFontSize(FONT_SIZE * 72 / 96);
  expression.setFontColor(color);
  if (!expression.parse("$" + (display ? QStringLiteral("\\displaystyle ") : QString{}) + latex + "$"))
    return {};
  if (!expression.getErrorList().isEmpty()) return {};

  QImage metrics(1, 1, QImage::Format_ARGB32_Premultiplied);
  metrics.setDotsPerMeterX(qRound(96 / 0.0254));
  metrics.setDotsPerMeterY(qRound(96 / 0.0254));
  QPainter measure(&metrics);
  double width, ascent, descent, strikeout;
  expression.getSizeDetail(measure, width, ascent, descent, strikeout);
  measure.end();

  if (!std::isfinite(width) || !std::isfinite(ascent) || !std::isfinite(descent) ||
      !std::isfinite(strikeout) || width <= 0 || width > MAX_IMAGE_PIXELS || ascent < 0 || descent < 0 ||
      ascent + descent > MAX_IMAGE_PIXELS)
    return {};

  // Inline images align around the text's middle; retain room on both sides of the math axis.
  const qreal halfHeight = std::max(ascent - strikeout, descent + strikeout) + 1;
  const QSizeF logicalSize(std::ceil(width + 2), std::ceil(display ? ascent + descent + 2 : halfHeight * 2));
  if (logicalSize.width() * logicalSize.height() > MAX_IMAGE_PIXELS) return {};

  QByteArray svg;
  QBuffer buffer(&svg);
  buffer.open(QIODevice::WriteOnly);
  QSvgGenerator drawing;
  drawing.setOutputDevice(&buffer);
  drawing.setResolution(96);
  drawing.setSize(logicalSize.toSize());
  drawing.setViewBox(QRectF(QPointF{}, logicalSize));
  QPainter painter(&drawing);
  expression.draw(painter, 1, display ? ascent + 1 : halfHeight + strikeout);
  painter.end();

  auto result = std::make_shared<Resource>(url, logicalSize / FONT_SIZE, std::move(svg));
  std::shared_ptr<const Resource> retained = result;
  {
    const std::lock_guard lock(resourceMutex);
    auto &entry = resources[QUrl(url)];
    if (const auto existing = entry.lock())
      retained = existing;
    else
      entry = retained;
  }
  const std::lock_guard lock(cacheMutex);
  const auto cost = (key.size() + url.size()) * sizeof(QChar) + retained->svg.size();
  cache.insert(key, new std::shared_ptr<const Resource>(retained), cost);
  return retained;
}

std::optional<QSizeF> math::size(const QUrl &url, qreal fontSize) {
  if (url.scheme() != "vicinae-math") return {};
  const auto source = resource(url);
  if (!source) return {};
  const auto size = source->size * fontSize;
  if (size.isEmpty() || !std::isfinite(size.width()) || !std::isfinite(size.height()) ||
      size.width() > MAX_IMAGE_PIXELS || size.height() > MAX_IMAGE_PIXELS)
    return {};
  return size;
}

void math::draw(QPainter *painter, const QRectF &rect, const QUrl &url) {
  const RasterKey key{url, rect.size(), painter->device()->devicePixelRatioF()};
  QImage image;
  {
    const std::lock_guard lock(rasterCacheMutex);
    if (const auto *cached = rasterCache.object(key)) image = *cached;
  }
  if (image.isNull()) {
    const auto pixels = (key.size * key.devicePixelRatio).toSize();
    if (pixels.isEmpty() || qint64(pixels.width()) * pixels.height() > MAX_IMAGE_PIXELS) return;
    image = QImage(pixels, QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(key.devicePixelRatio);
    image.fill(Qt::transparent);
    const auto source = resource(url);
    if (!source) return;
    QSvgRenderer drawing(source->svg);
    QPainter render(&image);
    drawing.render(&render, QRectF(QPointF{}, key.size));
    render.end();
    const auto cost =
        (image.sizeInBytes() + source->url.size() * sizeof(QChar) + sizeof(RasterKey)) / 1024 + 1;
    const std::lock_guard lock(rasterCacheMutex);
    rasterCache.insert(key, new QImage(image), cost);
  }
  painter->drawImage(rect.topLeft(), image);
}
