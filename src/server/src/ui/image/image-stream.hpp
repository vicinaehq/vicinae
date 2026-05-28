#pragma once
#include "ui/image/url.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <QFuture>
#include <QImage>
#include <QObject>
#include <QSize>
#include <atomic>
#include <memory>

class FetchReply;
class QMovie;

struct ImageStreamOptions {
  bool safetyMargins = false;
  bool cache = true;
};

class ImageStream : public QObject {
  Q_OBJECT

signals:
  void frameReady(const QImage &frame);
  void failed();

public:
  using Options = ImageStreamOptions;

  ImageStream(const ImageURL &url, const QSize &size, Options opts = {}, QObject *parent = nullptr);
  ~ImageStream() override;

  // Looks up the cache; if hit, emits frameReady synchronously and returns true.
  // Otherwise dispatches the rendering pipeline asynchronously and returns false.
  // Connect to frameReady/failed before calling.
  bool start();

private:
  void startStatic();
  void startFetchable();
  void dispatch();
  void tryFallback();

  void onDataReceived(const QByteArray &data);
  void decodeStatic(const QByteArray &data);
  void startAnimation(QByteArray data);
  void emitStaticFrame(QImage img);
  void handleStaticFuture(QFuture<QImage> future);

  ImageURL m_url;
  QSize m_size;
  QColor m_fg;
  OmniPainter::ImageMaskType m_mask = OmniPainter::NoMask;
  QString m_cacheKey;
  QString m_originalCacheKey;
  int m_fallbacksRemaining = 2;
  Options m_opts;

  QMovie *m_movie = nullptr;
  FetchReply *m_pendingReply = nullptr;
  std::shared_ptr<std::atomic<bool>> m_canceled = std::make_shared<std::atomic<bool>>(false);
};
