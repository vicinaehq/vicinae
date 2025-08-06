#include "http-image-loader.hpp"
#include "image-fetcher.hpp"
#include <qbuffer.h>

void HttpImageLoader::render(const RenderConfig &cfg) {
  if (m_reply) { m_reply->abort(); }

  QNetworkRequest request(m_url);
  auto reply = NetworkFetcher::instance()->fetch(m_url);

  // important: we connect to the current reply, not m_reply
  m_reply = reply;

  connect(reply, &FetchReply::finished, this, [this, reply, cfg](const QByteArray &data) {
    if (m_reply != reply) return;

    m_loader.reset(new IODeviceImageLoader(data));
    m_loader->forwardSignals(this);
    m_loader->render(cfg);
    if (m_reply == reply) { m_reply = nullptr; }
    reply->deleteLater();
  });
}

HttpImageLoader::HttpImageLoader(const QUrl &url) : m_url(url) {}

HttpImageLoader::~HttpImageLoader() {
  if (m_reply) {
    m_reply->abort();
    m_reply->deleteLater();
  }
}
