#include "animated-image-loader.hpp"
#include "ui/image/image.hpp"
#include <QMovie>

void AnimatedIODeviceImageLoader::render(const RenderConfig &cfg) {
  QSize deviceSize = cfg.size * cfg.devicePixelRatio;

  m_movie = std::make_unique<QMovie>();
  m_movie->setDevice(m_device.get());

  // m_movie->setScaledSize({deviceSize.width(), -1});
  m_movie->setCacheMode(QMovie::CacheAll);
  connect(m_movie.get(), &QMovie::updated, this, [this, deviceSize, cfg]() {
    auto pix = m_movie->currentPixmap();

    QSize originalSize = pix.size();
    bool isDownScalable =
        originalSize.height() > deviceSize.height() || originalSize.width() > deviceSize.width();

    if (isDownScalable) {
      Qt::AspectRatioMode ar = cfg.fit == ObjectFitFill ? Qt::IgnoreAspectRatio : Qt::KeepAspectRatio;

      m_movie->setScaledSize(originalSize.scaled(deviceSize, ar));
      emit dataUpdated(pix.scaled(deviceSize, ar));
      return;
    }

    pix.setDevicePixelRatio(cfg.devicePixelRatio);
    emit dataUpdated(pix);
  });
  m_movie->start();
}

AnimatedIODeviceImageLoader::AnimatedIODeviceImageLoader(std::unique_ptr<QIODevice> device)
    : m_device(std::move(device)) {}
