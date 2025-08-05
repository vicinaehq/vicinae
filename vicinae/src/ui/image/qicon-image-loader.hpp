#pragma once
#include "ui/image/image.hpp"

class QIconImageLoader : public AbstractImageLoader {
  QString m_icon;
  std::optional<QString> m_theme;

public:
  void render(const RenderConfig &config) override;

  QIconImageLoader(const QString &name, const std::optional<QString> &themeName = "");
};
