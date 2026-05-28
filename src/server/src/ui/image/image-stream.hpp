#pragma once
#include "ui/image/url.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include <QImage>
#include <QObject>
#include <QSize>
#include <atomic>
#include <memory>
#include <optional>

class FetchReply;
class QMovie;

class ImageStream : public QObject {
  Q_OBJECT

signals:
  void frameReady(const QImage &frame);
  void failed();

public:
  ImageStream(const ImageURL &url, const QSize &size, bool safetyMargins = false, QObject *parent = nullptr);
  ~ImageStream() override;

  static std::optional<QImage> findCached(const ImageURL &url, const QSize &size, bool safetyMargins = false);

private:
  void startStatic();
  void startFetchable();
  void dispatch();
  void tryFallback();

  void onDataReceived(const QByteArray &data);
  void decodeStatic(const QByteArray &data);
  void startAnimation(QByteArray data);
  void emitStaticFrame(QImage img);

  ImageURL m_url;
  QSize m_size;
  QColor m_fg;
  OmniPainter::ImageMaskType m_mask = OmniPainter::NoMask;
  QString m_cacheKey;
  int m_fallbacksRemaining = 2;
  bool m_safetyMargins = false;

  QMovie *m_movie = nullptr;
  FetchReply *m_pendingReply = nullptr;
  std::shared_ptr<std::atomic<bool>> m_canceled = std::make_shared<std::atomic<bool>>(false);
};
