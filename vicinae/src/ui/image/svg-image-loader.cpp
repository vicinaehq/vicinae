#include "ui/image/image.hpp"
#include "svg-image-loader.hpp"

void SvgImageLoader::render(QPixmap &pixmap, const QRect &bounds) {
  auto svgSize = m_renderer.defaultSize();
  // QRect targetRect = QRect(QPoint(0, 0), svgSize.scaled(bounds.size(), Qt::KeepAspectRatio));

  QPixmap filledSvg(bounds.size());

  filledSvg.fill(Qt::transparent);

  // first, we paint the filled svg on a separate pixmap
  {
    OmniPainter painter(&filledSvg);

    m_renderer.render(&painter, filledSvg.rect());

    if (m_fill) {
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
      painter.fillRect(filledSvg.rect(), *m_fill);
    }
  }

  QPainter painter(&pixmap);

  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.drawPixmap(bounds, filledSvg);
}

void SvgImageLoader::render(const RenderConfig &config) {
  QPixmap pixmap(config.size * config.devicePixelRatio);

  render(pixmap, pixmap.rect());
  pixmap.setDevicePixelRatio(config.devicePixelRatio);
  emit dataUpdated(pixmap);
}

void SvgImageLoader::setFillColor(const std::optional<ColorLike> &color) { m_fill = color; }

SvgImageLoader::SvgImageLoader(const QByteArray &data) { m_renderer.load(data); }
SvgImageLoader::SvgImageLoader(const QString &filename) { m_renderer.load(filename); }
