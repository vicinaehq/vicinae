#include "static-image-loader.hpp"
#include <qimagereader.h>
#include <QtConcurrent/QtConcurrent>

QImage StaticIODeviceImageLoader::loadStatic(std::unique_ptr<QIODevice> device, const RenderConfig &cfg) {
  QSize deviceSize = cfg.size * cfg.devicePixelRatio;
  QImageReader reader(device.get());
  QSize originalSize = reader.size();
  bool isDownScalable =
      originalSize.height() > deviceSize.height() || originalSize.width() > deviceSize.width();

  if (originalSize.isValid() && isDownScalable) {
    reader.setScaledSize(originalSize.scaled(deviceSize, cfg.fit == ObjectFitFill ? Qt::IgnoreAspectRatio
                                                                                  : Qt::KeepAspectRatio));
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
  if (m_started) return;

  m_started = true;
  auto future = QtConcurrent::run([cfg, this]() { return loadStatic(std::move(m_device), cfg); });
  m_watcher->setFuture(future);
}

StaticIODeviceImageLoader::StaticIODeviceImageLoader(std::unique_ptr<QIODevice> device)
    : m_device(std::move(device)), m_watcher(QSharedPointer<ImageWatcher>::create()) {
  connect(m_watcher.get(), &ImageWatcher::finished, this,
          [this]() { emit dataUpdated(QPixmap::fromImage(m_watcher->future().takeResult())); });
}

StaticIODeviceImageLoader::~StaticIODeviceImageLoader() {
  if (m_watcher && m_watcher->isRunning()) { m_watcher->cancel(); }
}
