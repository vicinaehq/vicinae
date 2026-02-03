#include "favicon-image-loader.hpp"
#include "favicon/favicon-service.hpp"
#include "ui/image/image.hpp"

void FaviconImageLoader::render(const RenderConfig &config) {
  if (m_watcher.isRunning()) { m_watcher.cancel(); }

  m_watcher.setFuture(FaviconService::instance()->makeRequest(m_domain));
  m_config = config;
}

void FaviconImageLoader::abort() const {}

FaviconImageLoader::FaviconImageLoader(const QString &hostname) : m_domain(hostname) {
  connect(&m_watcher, &Watcher::finished, this, [this]() {
    if (!m_watcher.isFinished()) return;

    auto result = m_watcher.result();

    if (!result) {
      emit errorOccured("Failed to load image");
      return;
    }

    auto scaled = result.value().scaled(m_config.size * m_config.devicePixelRatio, Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

    scaled.setDevicePixelRatio(m_config.devicePixelRatio);
    emit dataUpdated(scaled);
  });
}
