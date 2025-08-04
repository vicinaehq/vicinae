#pragma once
#include "ui/image/image.hpp"
#include "io-image-loader.hpp"

class FetchReply;

class HttpImageLoader : public AbstractImageLoader {
  std::unique_ptr<IODeviceImageLoader> m_loader;
  FetchReply *m_reply = nullptr;
  QUrl m_url;

  void render(const RenderConfig &cfg) override;

public:
  HttpImageLoader(const QUrl &url);
  ~HttpImageLoader();
};
