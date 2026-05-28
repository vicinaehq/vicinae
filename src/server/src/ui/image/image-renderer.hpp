#pragma once
#include "ui/omni-painter/omni-painter.hpp"
#include <QFuture>
#include <QImage>
#include <QSize>

class ImageURL;
class QThread;

namespace ImageRendering {

QFuture<QImage> render(const ImageURL &url, const QSize &size);

QImage decodeAndTransform(const QByteArray &data, const QSize &size, const QColor &fg = {},
                          OmniPainter::ImageMaskType mask = OmniPainter::NoMask);

void applyPostTransforms(QImage &image, const QColor &fg, OmniPainter::ImageMaskType mask);

QThreadPool &decodingPool();

QThread &animationThread();

} // namespace ImageRendering
