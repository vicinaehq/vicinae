#pragma once
#include "ui/image/image.hpp"

class QIconImageLoader : public AbstractImageLoader {
  QIcon m_icon;

public:
  void render(const RenderConfig &config) override;

  QIconImageLoader(const QString &name);
};
