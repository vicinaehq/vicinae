#pragma once
#include "ui/omni-painter/omni-painter.hpp"
#include <QFuture>
#include <QImage>
#include <QSize>
#include <QString>

class ImageURL;
class QThread;
class QThreadPool;

namespace ImageRendering {

QFuture<QImage> renderFirstFrame(const ImageURL &url, const QSize &size, bool safetyMargins = false);

QImage renderBuiltinSvg(const QString &name, const QSize &size, const QColor &fg, const QColor &bg);
QImage renderEmoji(const QString &emoji, const QSize &size);
QImage renderSystemIcon(const QString &name, const QSize &size);
QImage renderFileIcon(const QString &path, const QSize &size, const QColor &fg, const QColor &bg);
QFuture<QImage> renderFavicon(const QString &domain, const QSize &size, const QColor &fg,
                              OmniPainter::ImageMaskType mask);

QImage decodeImageData(QIODevice *device, const QSize &size);
QImage decodeImageData(const QByteArray &data, const QSize &size);
QImage decodeAndTransform(const QByteArray &data, const QSize &size, const QColor &fg = {},
                          OmniPainter::ImageMaskType mask = OmniPainter::NoMask);

void applyPostTransforms(QImage &image, const QColor &fg, OmniPainter::ImageMaskType mask);
void applySafetyMargins(QImage &image);

QThreadPool &decodingPool();
QThread &animationThread();

void clearCache();

} // namespace ImageRendering
