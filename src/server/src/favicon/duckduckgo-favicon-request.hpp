#pragma once
#include <QUrl>
#include "favicon/favicon-request.hpp"
#include "services/image-fetcher/image-fetcher.hpp"

class DuckDuckGoFaviconRequester : public AbstractFaviconRequest {
  FetchReply *m_currentReply = nullptr;

  QUrl makeUrl() const;
  void imageLoaded(const QByteArray &data);

public:
  void start() override;

  explicit DuckDuckGoFaviconRequester(const QString &domain, QObject *parent = nullptr);
  ~DuckDuckGoFaviconRequester() override;
};
