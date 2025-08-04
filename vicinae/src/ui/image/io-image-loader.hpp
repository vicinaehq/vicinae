#pragma once
#include "image.hpp"
#include <qmimetype.h>
#include <qstringview.h>

class IODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<AbstractImageLoader> m_loader;
  QByteArray m_data;

  bool isAnimatableMimeType(const QMimeType &type) const;

public:
  void render(const RenderConfig &cfg) override;

  IODeviceImageLoader(QByteArray bytes);
};
