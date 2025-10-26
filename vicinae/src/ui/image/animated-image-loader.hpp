#pragma once
#include "image.hpp"
#include <QMovie>
#include <qbuffer.h>
#include <qstringview.h>

class AnimatedIODeviceImageLoader : public AbstractImageLoader {
public:
  void render(const RenderConfig &cfg) override;
  AnimatedIODeviceImageLoader(const QByteArray &bytes);

private:
  std::unique_ptr<QMovie> m_movie;
  QBuffer m_buf;
  QByteArray m_data;
};
