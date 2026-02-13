#pragma once
#include <QImage>
#include <QQuickImageProvider>

class QmlAsyncImageProvider : public QQuickAsyncImageProvider {
public:
  QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};
