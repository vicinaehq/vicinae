#include "static-image-loader.hpp"
#include "ui/image/url.hpp"
#include <qimagereader.h>
#include <QtConcurrent/QtConcurrent>
#include <qnamespace.h>
#include <qstringview.h>

QImage StaticIODeviceImageLoader::loadStatic(const QByteArray &bytes, const RenderConfig &cfg) {
  QSize deviceSize = cfg.size * cfg.devicePixelRatio;
  QBuffer buf;
  buf.setData(bytes);
  buf.open(QIODevice::ReadOnly);
  QImageReader reader(&buf);
  QSize originalSize = reader.size();
  bool isDownScalable =
      originalSize.height() > deviceSize.height() || originalSize.width() > deviceSize.width();

  if (originalSize.isValid() && isDownScalable) {
    reader.setScaledSize(originalSize.scaled(deviceSize, ImageURL::fitToAspectRatio(cfg.fit)));
  }

  auto image = reader.read();

  image.setDevicePixelRatio(cfg.devicePixelRatio);

  return image;
}

void StaticIODeviceImageLoader::abort() const {
  if (m_watcher) m_watcher->cancel();
}

void StaticIODeviceImageLoader::render(const RenderConfig &cfg) {
  // TODO: allow rendering with a new config instead of no op
  auto future = QtConcurrent::run([cfg, data = m_data, this]() { return loadStatic(data, cfg); });
  m_watcher->setFuture(future);
}

StaticIODeviceImageLoader::StaticIODeviceImageLoader(const QByteArray &data)
    : m_data(data), m_watcher(QSharedPointer<ImageWatcher>::create()) {
  connect(m_watcher.get(), &ImageWatcher::finished, this,
          [this]() { emit dataUpdated(QPixmap::fromImage(m_watcher->future().takeResult())); });
}

StaticIODeviceImageLoader::~StaticIODeviceImageLoader() {
  if (m_watcher && m_watcher->isRunning()) { m_watcher->cancel(); }
}
