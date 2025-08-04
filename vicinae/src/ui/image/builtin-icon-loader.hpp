#pragma once
#include "ui/image/image.hpp"

class BuiltinIconLoader : public AbstractImageLoader {
  std::optional<ColorLike> m_backgroundColor;
  std::optional<ColorLike> m_fillColor;
  QString m_iconName;

  void render(const RenderConfig &config) override;

public:
  void setFillColor(const std::optional<ColorLike> &color);
  void setBackgroundColor(const std::optional<ColorLike> &color);

  BuiltinIconLoader(const QString &iconName);
};
