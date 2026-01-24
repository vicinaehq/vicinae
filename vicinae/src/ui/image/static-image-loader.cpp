#include "static-image-loader.hpp"

void StaticIODeviceImageLoader::abort() const {
  if (m_res) { BackgroundImageDecoder::instance()->cancel(m_res->id()); }
}

void StaticIODeviceImageLoader::render(const RenderConfig &cfg) {
  // TODO: allow rendering with a new config instead of no op
  m_res = BackgroundImageDecoder::instance()->decode(std::move(m_data), cfg);
  connect(m_res.get(), &BackgroundImageDecodeResponse::dataDecoded, this,
          [this](QPixmap pixmap) { emit dataUpdated(pixmap); });
}

StaticIODeviceImageLoader::StaticIODeviceImageLoader(QByteArray data) : m_data(data) {}
