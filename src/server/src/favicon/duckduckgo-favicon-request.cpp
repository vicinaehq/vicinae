#include "favicon/duckduckgo-favicon-request.hpp"

void DuckDuckGoFaviconRequester::imageLoaded(const QByteArray &data) {
  if (m_currentReply) {
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
  }

  if (data.isEmpty()) {
    emit failed();
    return;
  }

  emit finished(data);
}

QUrl DuckDuckGoFaviconRequester::makeUrl() const {
  return QUrl(QStringLiteral("https://icons.duckduckgo.com/ip3/%1.ico").arg(domain()));
}

void DuckDuckGoFaviconRequester::start() {
  if (m_currentReply) {
    m_currentReply->abort();
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
  }

  if (domain().isEmpty()) {
    emit failed();
    return;
  }

  auto *reply = NetworkFetcher::instance()->fetch(makeUrl());
  connect(reply, &FetchReply::finished, this, &DuckDuckGoFaviconRequester::imageLoaded);
  m_currentReply = reply;
}

DuckDuckGoFaviconRequester::DuckDuckGoFaviconRequester(const QString &domain, QObject *parent)
    : AbstractFaviconRequest(domain, parent) {}

DuckDuckGoFaviconRequester::~DuckDuckGoFaviconRequester() {
  if (m_currentReply) {
    m_currentReply->abort();
    m_currentReply->deleteLater();
  }
}
