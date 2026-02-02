#include "theme.hpp"
#include "ui/image/image.hpp"
#include <qnamespace.h>
#include "svg-image-loader.hpp"

void SvgImageLoader::render(QPixmap &pixmap, const QRect &bounds, const std::optional<ColorLike> &fill) {
  auto svgSize = m_renderer.defaultSize();
  // QRect targetRect = QRect(QPoint(0, 0), svgSize.scaled(bounds.size(), Qt::KeepAspectRatio));
  QPixmap filledSvg(bounds.size());

  filledSvg.fill(Qt::transparent);

  // first, we paint the filled svg on a separate pixmap
  {
    OmniPainter painter(&filledSvg);

    m_renderer.setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);
    m_renderer.render(&painter, filledSvg.rect());

    if (fill) {
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(filledSvg.rect(), *fill);
    }
  }

  QPainter painter(&pixmap);

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawPixmap(bounds, filledSvg);
}

void SvgImageLoader::render(const RenderConfig &config) {
  QPixmap pixmap(config.size * config.devicePixelRatio);

  pixmap.fill(Qt::transparent);
  render(pixmap, pixmap.rect(), config.fill);
  pixmap.setDevicePixelRatio(config.devicePixelRatio);
  emit dataUpdated(pixmap);
}

SvgImageLoader::SvgImageLoader(const QByteArray &data) { m_renderer.load(data); }
SvgImageLoader::SvgImageLoader(const QString &filename) { m_renderer.load(filename); }
