#pragma once
#include <QBuffer>
#include <QMovie>
#include <QQuickPaintedItem>
#include <QtQml/qqmlregistration.h>

class FetchReply;

class ViciAnimatedImage : public QQuickPaintedItem {
  Q_OBJECT
  QML_NAMED_ELEMENT(ViciAnimatedImage)
  Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
  Q_PROPERTY(bool animated READ isAnimated NOTIFY animatedChanged)

signals:
  void sourceChanged();
  void animatedChanged();

public:
  explicit ViciAnimatedImage(QQuickItem *parent = nullptr);
  ~ViciAnimatedImage() override;

  QString source() const;
  void setSource(const QString &src);
  bool isAnimated() const;

  void paint(QPainter *painter) override;

protected:
  void itemChange(ItemChange change, const ItemChangeData &value) override;

private:
  void reset();
  void loadData(const QByteArray &data);

  QString m_source;
  bool m_animated = false;
  QByteArray m_data;
  QBuffer *m_buffer = nullptr;
  QMovie *m_movie = nullptr;
  FetchReply *m_pendingReply = nullptr;
};
