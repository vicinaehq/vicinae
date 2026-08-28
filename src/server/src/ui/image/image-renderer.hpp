#pragma once
#include "ui/omni-painter/omni-painter.hpp"
#include <QFuture>
#include <QImage>
#include <QSize>
#include <QString>

#include <optional>

class ImageURL;
class QThread;
class QThreadPool;

namespace ImageRendering {

QFuture<QImage> renderFirstFrame(const ImageURL &url, const QSize &size, bool safetyMargins = false);
std::optional<QImage> cachedFrame(const ImageURL &url);

QImage renderBuiltinSvg(const QString &name, const QSize &size);
QImage renderEmoji(const QString &emoji, const QSize &size);
QImage renderSymbol(const QString &symbol, const QSize &size);
QImage renderFontPreview(const QString &spec, const QSize &size);
QImage renderSystemIcon(const QString &name, const QSize &size);
QImage renderFileIcon(const QString &path, const QSize &size, const QColor &fg);
QFuture<QImage> renderFavicon(const QString &domain, const QSize &size, const QColor &fg, const QColor &bg,
                              OmniPainter::ImageMaskType mask);

QImage decodeImageData(QIODevice *device, const QSize &size);
QImage decodeImageData(const QByteArray &data, const QSize &size);
QImage decodeAndTransform(const QByteArray &data, const QSize &size, const QColor &fg = {},
                          const QColor &bg = {}, OmniPainter::ImageMaskType mask = OmniPainter::NoMask);

// Size content should be rendered at when it will be composed onto a backdrop
// tile of the given full size.
QSize backdropContentSize(const QSize &size);

void applyPostTransforms(QImage &image, const QColor &fg, const QColor &bg, const QSize &size,
                         OmniPainter::ImageMaskType mask);
void applySafetyMargins(QImage &image);

QThreadPool &decodingPool();
QThread &animationThread();

void clearCache();

} // namespace ImageRendering
