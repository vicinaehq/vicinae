#include "http-image-loader.hpp"
#include "image-fetcher.hpp"
#include "timer.hpp"
#include <qbuffer.h>

void HttpImageLoader::render(const RenderConfig &cfg) {
  if (m_reply) { m_reply->abort(); }

  QNetworkRequest request(m_url);
  // auto reply = NetworkManager::instance()->manager()->get(request);
  auto reply = NetworkFetcher::instance()->fetch(m_url);

  // important: we connect to the current reply, not m_reply
  m_reply = reply;

  connect(reply, &FetchReply::finished, this, [this, reply, cfg](const QByteArray &data) {
    if (m_reply != reply) return;

    Timer timer;
    timer.time("read http image network data");
    m_loader = std::make_unique<IODeviceImageLoader>(data);
    connect(m_loader.get(), &IODeviceImageLoader::dataUpdated, this, &HttpImageLoader::dataUpdated);
    connect(m_loader.get(), &IODeviceImageLoader::errorOccured, this, &HttpImageLoader::errorOccured);
    m_loader->render(cfg);
    if (m_reply == reply) { m_reply = nullptr; }
    reply->deleteLater();
  });
}

HttpImageLoader::HttpImageLoader(const QUrl &url) : m_url(url) {}

HttpImageLoader::~HttpImageLoader() {
  if (m_reply) {
    // m_reply->blockSignals(true);
    m_reply->abort();
    m_reply->deleteLater();
  }
}
