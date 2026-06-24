#pragma once
#include "qml/image-url.hpp"
#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include <QtQml/qqmlregistration.h>

class ImageStream;

class ViciImageItem : public QQuickItem {
  Q_OBJECT
  QML_NAMED_ELEMENT(ViciImage)
  Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged)
  Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
  Q_PROPERTY(int fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
  Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged)
  Q_PROPERTY(bool safetyMargins READ safetyMargins WRITE setSafetyMargins NOTIFY safetyMarginsChanged)
  Q_PROPERTY(int status READ status NOTIFY statusChanged)

signals:
  void sourceChanged();
  void sourceSizeChanged();
  void fillModeChanged();
  void cacheChanged();
  void safetyMarginsChanged();
  void statusChanged();

public:
  enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop };
  Q_ENUM(FillMode)

  enum Status { Null, Ready, Loading, Error };
  Q_ENUM(Status)

  explicit ViciImageItem(QQuickItem *parent = nullptr);
  ~ViciImageItem() override;

  QVariant source() const;
  void setSource(const QVariant &src);

  QSize sourceSize() const { return m_sourceSize; }
  void setSourceSize(const QSize &size);

  int fillMode() const { return m_fillMode; }
  void setFillMode(int mode);

  bool cache() const { return m_cache; }
  void setCache(bool c);

  bool safetyMargins() const { return m_safetyMargins; }
  void setSafetyMargins(bool enabled);

  int status() const { return m_status; }

protected:
  QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;
  void geometryChange(const QRectF &newGeo, const QRectF &oldGeo) override;
  void itemChange(ItemChange change, const ItemChangeData &value) override;

private:
  void reload();
  void setStatus(Status s);

  QVariant m_source;
  ImageUrl m_resolvedUrl;
  QSize m_sourceSize;
  int m_fillMode = PreserveAspectFit;
  bool m_cache = true;
  bool m_safetyMargins = false;
  Status m_status = Null;

  ImageStream *m_stream = nullptr;
  QImage m_currentFrame;
  qint64 m_currentFrameKey = 0;
  QSGTexture *m_pendingTexture = nullptr;
  bool m_frameDirty = false;
  bool m_deferredReload = false;
};
