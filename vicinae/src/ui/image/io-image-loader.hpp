#pragma once
#include "image.hpp"
#include <qmimetype.h>

class IODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QIODevice> m_device;
  std::unique_ptr<AbstractImageLoader> m_loader;

  bool isAnimatableMimeType(const QMimeType &type) const;

public:
  void render(const RenderConfig &cfg) override;

  IODeviceImageLoader(std::unique_ptr<QIODevice> device);
};
