#pragma once
#include "ui/image/image.hpp"
#include <qsvgrenderer.h>

class SvgImageLoader : public AbstractImageLoader {
  QSvgRenderer m_renderer;
  std::optional<ColorLike> m_fill;

public:
  void render(QPixmap &pixmap, const QRect &bounds);
  void render(const RenderConfig &config) override;
  void setFillColor(const std::optional<ColorLike> &color);

  SvgImageLoader(const QByteArray &data);
  SvgImageLoader(const QString &filename);
};
