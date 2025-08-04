#include "io-image-loader.hpp"
#include "static-image-loader.hpp"
#include "animated-image-loader.hpp"
#include "ui/image/image.hpp"
#include <qmimedatabase.h>

bool IODeviceImageLoader::isAnimatableMimeType(const QMimeType &type) const {
  return type.name() == "image/gif";
}

void IODeviceImageLoader::render(const RenderConfig &cfg) {
  if (!m_device->isOpen()) {
    if (!m_device->open(QIODevice::ReadOnly)) {
      emit errorOccured(QString("IODevice could not be opened for reading"));
      return;
    }
  }

  QMimeDatabase mimeDb;
  QMimeType mime = mimeDb.mimeTypeForData(m_device.get());

  if (isAnimatableMimeType(mime)) {
    m_loader = std::make_unique<AnimatedIODeviceImageLoader>(std::move(m_device));
  } else {
    m_loader = std::make_unique<StaticIODeviceImageLoader>(std::move(m_device));
  }

  connect(m_loader.get(), &AbstractImageLoader::dataUpdated, this, &IODeviceImageLoader::dataUpdated);
  connect(m_loader.get(), &AbstractImageLoader::errorOccured, this, &IODeviceImageLoader::errorOccured);
  m_loader->render(cfg);
}

IODeviceImageLoader::IODeviceImageLoader(std::unique_ptr<QIODevice> device) : m_device(std::move(device)) {}
