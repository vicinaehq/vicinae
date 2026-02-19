#include "qml-animated-image.hpp"
#include "data-uri/data-uri.hpp"
#include "image-fetcher.hpp"
#include <QFile>
#include <QPainter>

static constexpr int kMaxAnimDataSize = 512 * 1024; // 512KB

ViciAnimatedImage::ViciAnimatedImage(QQuickItem *parent)
    : QQuickPaintedItem(parent) {}

ViciAnimatedImage::~ViciAnimatedImage() {
  if (m_pendingReply) {
    m_pendingReply->abort();
    m_pendingReply->deleteLater();
  }
  delete m_movie;
  delete m_buffer;
}

QString ViciAnimatedImage::source() const { return m_source; }
bool ViciAnimatedImage::isAnimated() const { return m_animated; }

void ViciAnimatedImage::reset() {
  if (m_pendingReply) {
    m_pendingReply->abort();
    m_pendingReply->deleteLater();
    m_pendingReply = nullptr;
  }

  delete m_movie;
  m_movie = nullptr;
  delete m_buffer;
  m_buffer = nullptr;
  m_data.clear();

  if (m_animated) {
    m_animated = false;
    emit animatedChanged();
    update();
  }
}

void ViciAnimatedImage::setSource(const QString &src) {
  if (m_source == src) return;
  m_source = src;
  emit sourceChanged();
  reset();

  if (src.isEmpty()) return;

  static const QString prefix = QStringLiteral("image://vicinae/");
  QString id = src.startsWith(prefix) ? src.mid(prefix.length()) : src;

  int colonIdx = id.indexOf(':');
  if (colonIdx < 0) return;

  QString type = id.left(colonIdx);
  QString rest = id.mid(colonIdx + 1);
  int qmark = rest.indexOf('?');
  QString name = qmark >= 0 ? rest.left(qmark) : rest;

  if (type == QStringLiteral("http")) {
    m_pendingReply = NetworkFetcher::instance()->fetch(QUrl(name));
    connect(m_pendingReply, &FetchReply::finished, this,
            [this](const QByteArray &data) {
              m_pendingReply->deleteLater();
              m_pendingReply = nullptr;
              loadData(data);
            });
  } else if (type == QStringLiteral("local")) {
    QFile f(name);
    if (f.open(QIODevice::ReadOnly))
      loadData(f.readAll());
  } else if (type == QStringLiteral("datauri")) {
    DataUri uri(name);
    loadData(uri.decodeContent());
  }
}

void ViciAnimatedImage::loadData(const QByteArray &data) {
  if (data.size() < 6) return;
  if (!data.startsWith("GIF87a") && !data.startsWith("GIF89a")) return;
  if (data.size() > kMaxAnimDataSize) return;

  m_data = data;
  m_buffer = new QBuffer(&m_data, this);
  m_buffer->open(QIODevice::ReadOnly);

  m_movie = new QMovie(this);
  m_movie->setDevice(m_buffer);
  m_movie->setCacheMode(QMovie::CacheAll);

  if (!m_movie->isValid() || m_movie->frameCount() == 1) {
    delete m_movie;
    m_movie = nullptr;
    delete m_buffer;
    m_buffer = nullptr;
    m_data.clear();
    return;
  }

  updateScaledSize();
  connect(m_movie, &QMovie::updated, this, [this]() { update(); });
  m_movie->start();

  m_animated = true;
  emit animatedChanged();
}

void ViciAnimatedImage::paint(QPainter *painter) {
  if (!m_movie || m_movie->state() != QMovie::Running) return;

  QPixmap frame = m_movie->currentPixmap();
  if (frame.isNull()) return;

  painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

  QSizeF itemSize(width(), height());
  QSizeF scaled = QSizeF(frame.size()).scaled(itemSize, Qt::KeepAspectRatio);
  QRectF target((itemSize.width() - scaled.width()) / 2.0,
                (itemSize.height() - scaled.height()) / 2.0,
                scaled.width(), scaled.height());

  painter->drawPixmap(target, frame, QRectF(frame.rect()));
}

void ViciAnimatedImage::itemChange(ItemChange change,
                                   const ItemChangeData &value) {
  QQuickPaintedItem::itemChange(change, value);
  if (change == ItemVisibleHasChanged && m_movie) {
    if (value.boolValue)
      m_movie->start();
    else
      m_movie->setPaused(true);
  }
}

void ViciAnimatedImage::geometryChange(const QRectF &newGeo,
                                       const QRectF &oldGeo) {
  QQuickPaintedItem::geometryChange(newGeo, oldGeo);
  if (newGeo.size() != oldGeo.size())
    updateScaledSize();
}

void ViciAnimatedImage::updateScaledSize() {
  if (!m_movie) return;
  int w = static_cast<int>(width());
  int h = static_cast<int>(height());
  if (w > 0 && h > 0)
    m_movie->setScaledSize(QSize(w, h));
}
