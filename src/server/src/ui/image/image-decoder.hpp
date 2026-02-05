#pragma once
#include <cstdint>
#include <deque>
#include <qimagereader.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <unordered_map>
#include <QtConcurrent/QtConcurrent>
#include "ui/image/image.hpp"
#include "ui/image/url.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "common/types.hpp"

class BackgroundImageDecodeResponse : public QObject {
  Q_OBJECT

signals:
  void dataDecoded(QPixmap pixmap) const;

public:
  using Handle = uint32_t;

  BackgroundImageDecodeResponse(Handle id) : m_id(id) {}
  Handle id() const { return m_id; }

private:
  Handle m_id;
};

/**
 * Singleton used to process image decoding requests, that cannot be performed
 * in the main thread for performance reasons.
 * Not thread-safe.
 */
class BackgroundImageDecoder : public QObject, NonCopyable {
public:
  using ResponsePtr = QSharedPointer<BackgroundImageDecodeResponse>;

private:
  using Watcher = QFutureWatcher<QImage>;
  using Handle = BackgroundImageDecodeResponse::Handle;

  struct ImageData {
    ResponsePtr res;
    QByteArray bytes;
    RenderConfig cfg;
  };

  struct JobData {
    QSharedPointer<Watcher> watcher = nullptr;
    QSharedPointer<BackgroundImageDecodeResponse> data;
  };

public:
  static BackgroundImageDecoder *instance() {
    static BackgroundImageDecoder decoder;
    return &decoder;
  }

  QSharedPointer<BackgroundImageDecodeResponse> decode(QByteArray &&data, const RenderConfig &cfg) {
    auto response = ResponsePtr::create(m_serial++);

    if (m_tasks.size() < MAX_CONCURRENT_JOBS) {
      enqueueJob(response, std::move(data), cfg);
      return response;
    }

    m_pending.emplace_back(ImageData(response, std::move(data), cfg));

    return response;
  }

  void cancel(Handle id) {
    if (auto it = m_tasks.find(id); it != m_tasks.end()) { it->second.watcher->cancel(); }
    if (auto it = std::ranges::find_if(m_pending, [id](auto &&img) { return img.res->id() == id; });
        it != m_pending.end()) {
      m_pending.erase(it);
    }
  }

private:
  static constexpr const int MAX_CONCURRENT_JOBS = 2;

  static QImage loadStatic(QByteArray bytes, const RenderConfig &cfg) {
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

    if (cfg.fill) {
      QPixmap pixmap = QPixmap::fromImage(image);
      QPixmap tintedPixmap(pixmap.size());
      tintedPixmap.fill(Qt::transparent);

      QPainter painter(&tintedPixmap);

      painter.setRenderHint(QPainter::Antialiasing, true);
      painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
      painter.drawPixmap(0, 0, pixmap);
      painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

      QColor fillColor = OmniPainter::resolveColor(*cfg.fill);
      painter.fillRect(tintedPixmap.rect(), fillColor);

      image = tintedPixmap.toImage();
    }

    image.setDevicePixelRatio(cfg.devicePixelRatio);

    return image;
  }

  void processNext() {
    while (!m_pending.empty() && m_tasks.size() < MAX_CONCURRENT_JOBS) {
      auto data = m_pending.front();
      m_pending.pop_front();
      enqueueJob(data.res, std::move(data.bytes), data.cfg);
    }
  }

  void enqueueJob(ResponsePtr res, QByteArray &&data, const RenderConfig &cfg) {
    auto watcher = QSharedPointer<Watcher>::create();
    watcher->setFuture(QtConcurrent::run([data = std::move(data), cfg]() { return loadStatic(data, cfg); }));

    auto id = res->id();
    auto *watcherPtr = watcher.get();
    connect(watcherPtr, &Watcher::finished, this, [this, id, watcherPtr]() {
      if (watcherPtr->isFinished() && !watcherPtr->isCanceled()) {
        emit m_tasks[id].data->dataDecoded(QPixmap::fromImage(watcherPtr->future().takeResult()));
      }
      m_tasks.erase(id);
      processNext();
    });

    m_tasks[res->id()] = JobData(watcher, res);
  }

  std::unordered_map<uint32_t, JobData> m_tasks;
  std::deque<ImageData> m_pending;
  Handle m_serial = 0;
};
