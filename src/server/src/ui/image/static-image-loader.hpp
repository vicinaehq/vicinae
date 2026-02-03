#pragma once
#include "ui/image/image-decoder.hpp"
#include "ui/image/image.hpp"

class StaticIODeviceImageLoader : public AbstractImageLoader {
public:
  void abort() const override;
  void render(const RenderConfig &cfg) override;

  StaticIODeviceImageLoader(QByteArray data);

private:
  QByteArray m_data;
  BackgroundImageDecoder::ResponsePtr m_res;
};
