#include "io-image-loader.hpp"
#include "static-image-loader.hpp"
#include "animated-image-loader.hpp"
#include "ui/image/image.hpp"
#include <qmimedatabase.h>
#include <qstringview.h>

bool IODeviceImageLoader::isAnimatableMimeType(const QMimeType &type) const {
  return type.name() == "image/gif";
}

void IODeviceImageLoader::render(const RenderConfig &cfg) {
  QMimeDatabase mimeDb;
  QMimeType mime = mimeDb.mimeTypeForData(m_data);

  if (isAnimatableMimeType(mime)) {
    m_loader = std::make_unique<AnimatedIODeviceImageLoader>(m_data);
  } else {
    m_loader = std::make_unique<StaticIODeviceImageLoader>(m_data);
  }

  m_loader->forwardSignals(this);
  m_loader->render(cfg);
}

IODeviceImageLoader::IODeviceImageLoader(QByteArray bytes) : m_data(bytes) {}
