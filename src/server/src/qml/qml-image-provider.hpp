#pragma once
#include <QQuickImageProvider>

class QmlImageProvider : public QQuickImageProvider {
public:
  QmlImageProvider();
  QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};
