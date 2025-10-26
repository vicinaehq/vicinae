#include "animated-image-loader.hpp"
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include <QMovie>
#include <qbuffer.h>
#include <qnamespace.h>
#include <qstringview.h>

void AnimatedIODeviceImageLoader::render(const RenderConfig &cfg) {
  QSize deviceSize = cfg.size * cfg.devicePixelRatio;

  m_movie = std::make_unique<QMovie>();
  m_movie->setDevice(&m_buf);
  m_movie->setScaledSize({deviceSize.width(), -1});
  m_movie->setCacheMode(QMovie::CacheAll);

  connect(m_movie.get(), &QMovie::updated, this, [this, deviceSize, cfg]() {
    auto pix = m_movie->currentPixmap();
    QSize frameSize = pix.size();
    pix.setDevicePixelRatio(cfg.devicePixelRatio);
    auto ar = ImageURL::fitToAspectRatio(cfg.fit);
    auto size = frameSize.scaled(deviceSize, ar);
    emit dataUpdated(pix.scaled(size, ar, Qt::SmoothTransformation), false);
  });
  m_movie->start();
}

AnimatedIODeviceImageLoader::AnimatedIODeviceImageLoader(const QByteArray &data) : m_data(data) {
  m_buf.setData(m_data);
  m_buf.open(QIODevice::ReadOnly);
}
