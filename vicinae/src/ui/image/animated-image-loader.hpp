#pragma once
#include "image.hpp"
#include <QMovie>

class AnimatedIODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QMovie> m_movie;
  std::unique_ptr<QIODevice> m_device = nullptr;

public:
  void render(const RenderConfig &cfg) override;
  AnimatedIODeviceImageLoader(std::unique_ptr<QIODevice> device);
};
