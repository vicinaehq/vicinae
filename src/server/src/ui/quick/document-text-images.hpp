#pragma once

#include <QCache>
#include <QImage>
#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QTextObjectInterface>
#include <QUrl>
#include <QtQml/qqmlregistration.h>
#include <optional>

class QQmlEngine;

class DocumentImageCache : public QObject {
  Q_OBJECT

signals:
  void sizesChanged();

public:
  explicit DocumentImageCache(QObject *parent);
  static DocumentImageCache *forEngine(QQmlEngine *engine);
  QImage image(const QUrl &url) const;
  std::optional<QSize> size(const QUrl &url) const;
  void remember(const QUrl &url, const QImage &image);

private:
  QCache<QUrl, QImage> m_images;
  QCache<QUrl, QSize> m_sizes;
  bool m_notificationPending = false;
};

class DocumentTextImages : public QObject, public QTextObjectInterface {
  Q_OBJECT
  QML_ELEMENT
  Q_INTERFACES(QTextObjectInterface)
  Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
  Q_PROPERTY(bool measuring READ measuring WRITE setMeasuring NOTIFY measuringChanged)
  Q_PROPERTY(
      qreal devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio NOTIFY devicePixelRatioChanged)

signals:
  void documentChanged();
  void measuringChanged();
  void devicePixelRatioChanged();

public:
  explicit DocumentTextImages(QObject *parent = nullptr);
  ~DocumentTextImages() override;
  QQuickTextDocument *document() const { return m_document; }
  void setDocument(QQuickTextDocument *document);
  bool measuring() const { return m_measuring; }
  void setMeasuring(bool measuring);
  qreal devicePixelRatio() const { return m_devicePixelRatio; }
  void setDevicePixelRatio(qreal ratio);
  static void measure(QTextDocument *document, DocumentImageCache *cache);
  QSizeF intrinsicSize(QTextDocument *document, int position, const QTextFormat &format) override;
  void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *document, int position,
                  const QTextFormat &format) override;

private:
  void attach(QTextDocument *document);
  QPointer<QQuickTextDocument> m_document;
  QPointer<QTextDocument> m_textDocument;
  QPointer<DocumentImageCache> m_cache;
  bool m_measuring = false;
  qreal m_devicePixelRatio = 1;
};
