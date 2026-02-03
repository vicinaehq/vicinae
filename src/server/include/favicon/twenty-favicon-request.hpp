#pragma once
#include "favicon-request.hpp"
#include "image-fetcher.hpp"
#include <qstringview.h>

class TwentyFaviconRequester : public AbstractFaviconRequest {
  const std::vector<uint> sizes = {128, 64, 32, 16};
  size_t currentSizeAttemptIndex = 0;
  QString placeholderUrl;
  FetchReply *_currentReply;

  QString makeUrl(uint size) const;
  void loadingFailed();
  void imageLoaded(const QByteArray &data);
  void tryForCurrentSize();

public:
  void start() override;

  TwentyFaviconRequester(const QString &domain, QObject *parent = nullptr);
  ~TwentyFaviconRequester();
};
