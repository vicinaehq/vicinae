#pragma once
#include "ui/image/image-decoder.hpp"
#include "ui/image/image.hpp"
#include <qbuffer.h>
#include <qfuturewatcher.h>
#include <qstringview.h>

class StaticIODeviceImageLoader : public AbstractImageLoader {
  using ImageWatcher = QFutureWatcher<QImage>;
  QSharedPointer<ImageWatcher> m_watcher;
  QByteArray m_data;

public:
  void abort() const override;
  void render(const RenderConfig &cfg) override;

public:
  StaticIODeviceImageLoader(const QByteArray &data);
  ~StaticIODeviceImageLoader();

private:
  BackgroundImageDecoder::ResponsePtr m_res;
};
