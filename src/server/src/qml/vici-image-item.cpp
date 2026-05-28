#include "vici-image-item.hpp"
#include "theme.hpp"
#include "ui/image/image-stream.hpp"
#include <QGuiApplication>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QtMath>

ViciImageItem::ViciImageItem(QQuickItem *parent) : QQuickItem(parent) {
  setFlag(ItemHasContents, true);
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (m_resolvedUrl.isThemeSensitive()) reload();
  });
}

ViciImageItem::~ViciImageItem() { delete m_pendingTexture; }

QVariant ViciImageItem::source() const { return m_source; }

void ViciImageItem::setSource(const QVariant &src) {
  if (m_source == src) return;
  m_source = src;
  emit sourceChanged();

  ImageUrl resolved;
  if (src.typeId() == QMetaType::QString) {
    auto str = src.toString();
    if (!str.isEmpty()) resolved = ImageUrl(ImageURL(str));
  } else if (src.canConvert<ImageUrl>()) {
    resolved = src.value<ImageUrl>();
  }

  m_resolvedUrl = resolved;
  reload();
}

void ViciImageItem::setSourceSize(const QSize &size) {
  if (m_sourceSize == size) return;
  m_sourceSize = size;
  emit sourceSizeChanged();
  reload();
}

void ViciImageItem::setFillMode(int mode) {
  if (m_fillMode == mode) return;
  m_fillMode = mode;
  emit fillModeChanged();
  update();
}

void ViciImageItem::setCache(bool c) {
  if (m_cache == c) return;
  m_cache = c;
  emit cacheChanged();
}

void ViciImageItem::setSafetyMargins(bool enabled) {
  if (m_safetyMargins == enabled) return;
  m_safetyMargins = enabled;
  emit safetyMarginsChanged();
  reload();
}

void ViciImageItem::setStatus(Status s) {
  if (m_status == s) return;
  m_status = s;
  emit statusChanged();
}

void ViciImageItem::reload() {
  m_deferredReload = false;

  delete m_stream;
  m_stream = nullptr;

  if (!m_resolvedUrl.isValid()) {
    delete m_pendingTexture;
    m_pendingTexture = nullptr;
    m_currentFrame = {};
    m_currentFrameKey = 0;
    m_frameDirty = true;
    setImplicitSize(0, 0);
    setStatus(Null);
    update();
    return;
  }

  int w = m_sourceSize.width() > 0 ? m_sourceSize.width() : qCeil(width());
  int h = m_sourceSize.height() > 0 ? m_sourceSize.height() : qCeil(height());
  if (w <= 0 || h <= 0) {
    m_deferredReload = true;
    polish();
    return;
  }

  qreal const dpr = window() ? window()->devicePixelRatio() : qGuiApp->devicePixelRatio();
  QSize const physicalSize(qCeil(w * dpr), qCeil(h * dpr));

  m_stream = new ImageStream(m_resolvedUrl.imageUrl(), physicalSize, m_safetyMargins, this);

  connect(m_stream, &ImageStream::frameReady, this, [this, dpr](const QImage &frame) {
    qint64 const key = frame.cacheKey();
    if (m_currentFrameKey == key && !m_currentFrame.isNull()) {
      setStatus(Ready);
      return;
    }
    QImage img = frame;
    img.setDevicePixelRatio(dpr);
    m_currentFrame = std::move(img);
    m_currentFrameKey = key;

    setImplicitSize(m_currentFrame.width() / dpr, m_currentFrame.height() / dpr);

    delete m_pendingTexture;
    m_pendingTexture = nullptr;
    if (window())
      m_pendingTexture = window()->createTextureFromImage(m_currentFrame, QQuickWindow::TextureHasMipmaps);

    m_frameDirty = true;
    setStatus(Ready);
    update();
  });

  connect(m_stream, &ImageStream::failed, this, [this]() { setStatus(Error); });

  if (!m_stream->start()) {
    delete m_pendingTexture;
    m_pendingTexture = nullptr;
    m_currentFrame = {};
    m_currentFrameKey = 0;
    m_frameDirty = true;
    setStatus(Loading);
    update();
  }
}

void ViciImageItem::updatePolish() {
  if (!m_deferredReload) return;
  int w = m_sourceSize.width() > 0 ? m_sourceSize.width() : qCeil(width());
  int h = m_sourceSize.height() > 0 ? m_sourceSize.height() : qCeil(height());
  if (w <= 0 || h <= 0) return;
  m_deferredReload = false;
  reload();
}

void ViciImageItem::itemChange(ItemChange change, const ItemChangeData &value) {
  QQuickItem::itemChange(change, value);
  if (change == ItemSceneChange && value.window && !m_currentFrame.isNull() && !m_pendingTexture) {
    m_pendingTexture = value.window->createTextureFromImage(m_currentFrame, QQuickWindow::TextureHasMipmaps);
    m_frameDirty = true;
    update();
  }
  if (change == ItemVisibleHasChanged && value.boolValue) {
    if (m_deferredReload) {
      reload();
    } else if (!m_currentFrame.isNull()) {
      m_frameDirty = true;
      update();
    }
  }
}

void ViciImageItem::geometryChange(const QRectF &newGeo, const QRectF &oldGeo) {
  QQuickItem::geometryChange(newGeo, oldGeo);
  if (newGeo.size() == oldGeo.size()) return;
  if (!m_sourceSize.isEmpty()) {
    update();
    return;
  }
  if (m_deferredReload) {
    m_deferredReload = false;
    reload();
    return;
  }
  if (!m_currentFrame.isNull()) {
    update();
    return;
  }
  if (m_resolvedUrl.isValid()) reload();
}

QSGNode *ViciImageItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
  if (m_currentFrame.isNull()) {
    delete oldNode;
    return nullptr;
  }

  auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);
  if (!node) {
    node = new QSGSimpleTextureNode();
    node->setOwnsTexture(true);
    node->setFiltering(QSGTexture::Linear);
  }

  if (m_frameDirty) {
    if (!m_pendingTexture && window())
      m_pendingTexture = window()->createTextureFromImage(m_currentFrame, QQuickWindow::TextureHasMipmaps);
    if (m_pendingTexture) {
      m_pendingTexture->setMipmapFiltering(QSGTexture::Linear);
      node->setTexture(m_pendingTexture);
      m_pendingTexture = nullptr;
      m_frameDirty = false;
    }
  }

  if (!node->texture()) {
    delete node;
    return nullptr;
  }

  QSizeF const itemSize(width(), height());
  QSizeF const imgSize(m_currentFrame.width() / m_currentFrame.devicePixelRatio(),
                       m_currentFrame.height() / m_currentFrame.devicePixelRatio());

  QRectF destRect(0, 0, itemSize.width(), itemSize.height());
  QRectF srcRect;

  if (m_fillMode == PreserveAspectFit) {
    QSizeF const scaled = imgSize.scaled(itemSize, Qt::KeepAspectRatio);
    destRect = QRectF((itemSize.width() - scaled.width()) / 2.0, (itemSize.height() - scaled.height()) / 2.0,
                      scaled.width(), scaled.height());
  } else if (m_fillMode == PreserveAspectCrop) {
    QSizeF const scaled = imgSize.scaled(itemSize, Qt::KeepAspectRatioByExpanding);
    qreal visibleW = itemSize.width() / scaled.width();
    qreal visibleH = itemSize.height() / scaled.height();
    qreal texW = m_currentFrame.width();
    qreal texH = m_currentFrame.height();
    srcRect = QRectF((1.0 - visibleW) / 2.0 * texW, (1.0 - visibleH) / 2.0 * texH, visibleW * texW,
                     visibleH * texH);
  }

  node->setRect(destRect);
  node->setSourceRect(srcRect);
  return node;
}
