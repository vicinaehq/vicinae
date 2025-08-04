#include "favicon-image-loader.hpp"
#include "favicon/favicon-service.hpp"
#include "ui/image/image.hpp"

void FaviconImageLoader::render(const RenderConfig &config) {
  auto reply = FaviconService::instance()->makeRequest(m_domain);

  m_requester = QSharedPointer<AbstractFaviconRequest>(reply);
  connect(m_requester.get(), &AbstractFaviconRequest::finished, this, [this, config](const QPixmap &pixmap) {
    auto scaled =
        pixmap.scaled(config.size * config.devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    scaled.setDevicePixelRatio(config.devicePixelRatio);
    emit dataUpdated(scaled);
  });
  connect(m_requester.get(), &AbstractFaviconRequest::failed, this,
          [this]() { emit errorOccured("Failed to load image"); });
  m_requester->start();
}

void FaviconImageLoader::abort() const {
  // TODO: implement proper request abort
}

FaviconImageLoader::FaviconImageLoader(const QString &hostname) : m_domain(hostname) {}
