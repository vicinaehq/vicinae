#pragma once
#include <QImage>
#include <QQuickImageProvider>

class AsyncImageProvider : public QQuickAsyncImageProvider {
public:
  QQuickImageResponse *requestImageResponse(const QString &id, const QSize &requestedSize) override;
};
