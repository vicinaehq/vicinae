#pragma once
#include "ui/omni-painter/omni-painter.hpp"
#include <QFuture>
#include <QImage>
#include <QSize>

class ImageURL;

namespace ImageRendering {

QFuture<QImage> render(const ImageURL &url, const QSize &size);

QImage renderBuiltinSvg(const QString &name, const QSize &size, const QColor &fg, const QColor &bg);
QImage renderEmoji(const QString &emoji, const QSize &size);
QImage renderSystemIcon(const QString &name, const QSize &size);
QImage renderLocalSvg(const QString &path, const QSize &size);
QImage renderLocalRaster(const QString &path, const QSize &size);
QImage decodeImageData(const QByteArray &data, const QSize &size);

void applyPostTransforms(QImage &image, const QColor &fg, OmniPainter::ImageMaskType mask);

QThreadPool &decodingPool();

} // namespace ImageRendering
