#pragma once
#include "theme.hpp"
#include "ui/image/image.hpp"
#include <qsvgrenderer.h>

class SvgImageLoader : public AbstractImageLoader {
  QSvgRenderer m_renderer;

public:
  void render(QPixmap &pixmap, const QRect &bounds, const std::optional<ColorLike> &fill);
  void render(const RenderConfig &config) override;

  SvgImageLoader(const QByteArray &data);
  SvgImageLoader(const QString &filename);
};
