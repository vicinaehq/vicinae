#pragma once

#include <memory>
#include <qdir.h>
#include <qfuturewatcher.h>
#include <qimage.h>
#include <QtConcurrent/QtConcurrent>
#include <qimagereader.h>
#include <qmimedatabase.h>
#include <qmovie.h>
#include <qobject.h>
#include <qpixmap.h>
#include <filesystem>

namespace Omnimg {
class AbstractImageLoader : public QObject {
  Q_OBJECT

  /**
   * Asks the loader to (re)load the image data and to start emitting
   * signals.
   */
  void virtual render(QSize size) = 0;
  void virtual abort() const {};

signals:
  void dataUpdated(const QPixmap &data) const;
  void errorOccured(const QString &errorDescription);
};

class IODeviceImageLoader : public AbstractImageLoader {
  std::unique_ptr<QMovie> m_movie;
  QIODevice *m_device;
  using PixWatcher = QFutureWatcher<QPixmap>;

  bool isAnimatableMimeType(const QMimeType &type) const { return type.name() == "image/gif"; }

  void renderMovie(QSize size) {
    m_movie = std::make_unique<QMovie>(m_device);
    m_movie->setScaledSize(size);
    connect(m_movie.get(), &QMovie::frameChanged, this,
            [this]() { emit dataUpdated(m_movie->currentPixmap()); });
    m_movie->start();
  }

  static QPixmap loadStatic(QIODevice *device, QSize size) {
    QImageReader reader(device);
    QSize originalSize = reader.size();
    bool isDownScalable = originalSize.height() > size.height() || originalSize.width() > size.width();

    if (originalSize.isValid() && isDownScalable) {
      reader.setScaledSize(originalSize.scaled(size, Qt::KeepAspectRatio));
    }

    auto pix = QPixmap::fromImageReader(&reader);

    pix.setDevicePixelRatio(1);

    return pix;
  }

  void renderStatic(QSize size) {
    auto watcher = new PixWatcher;
    auto future = QtConcurrent::run([size, this]() { return loadStatic(m_device, size); });

    watcher->setFuture(future);
    connect(watcher, &PixWatcher::finished, this, [this, watcher]() {
      emit dataUpdated(watcher->future().takeResult());
      watcher->deleteLater();
    });
  }

public:
  void render(QSize size) override {
    if (!m_device->isOpen()) {
      if (!m_device->open(QIODevice::ReadOnly)) {
        emit errorOccured(QString("IODevice could not be opened for reading"));
        return;
      }
    }

    QMimeDatabase mimeDb;
    QMimeType mime = mimeDb.mimeTypeForData(m_device);

    if (isAnimatableMimeType(mime)) {
      renderMovie(size);
      return;
    }

    return renderStatic(size);
  }

  IODeviceImageLoader(QIODevice *device) : m_device(device) {}
};

class LocalImageLoader : public AbstractImageLoader {
  std::filesystem::path m_path;
  std::unique_ptr<IODeviceImageLoader> m_loader;
  QFile m_file;

  void render(QSize size) override {
    m_loader = std::make_unique<IODeviceImageLoader>(&m_file);
    m_loader->render(size);
  }

public:
  LocalImageLoader(const std::filesystem::path &path) : m_path(path) { m_file.setFileName(path); }
};

}; // namespace Omnimg
