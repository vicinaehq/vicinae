#pragma once
#include "common.hpp"
#include "ui/image/image.hpp"
#include "io-image-loader.hpp"

class FetchReply;

class HttpImageLoader : public AbstractImageLoader {
  QObjectUniquePtr<IODeviceImageLoader> m_loader;
  FetchReply *m_reply = nullptr;
  QUrl m_url;

  void render(const RenderConfig &cfg) override;

public:
  HttpImageLoader(const QUrl &url);
  ~HttpImageLoader();
};
