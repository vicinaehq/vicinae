#pragma once
#include "ui/image/image.hpp"
#include <qpixmap.h>

class BuiltinIconLoader : public AbstractImageLoader {
  std::optional<ColorLike> m_backgroundColor;
  std::optional<ColorLike> m_fillColor;
  QString m_iconName;

public:
  void render(const RenderConfig &config) override;
  QPixmap renderSync(const RenderConfig &config);
  void setFillColor(const std::optional<ColorLike> &color);
  void setBackgroundColor(const std::optional<ColorLike> &color);

  BuiltinIconLoader(const QString &iconName);
};
