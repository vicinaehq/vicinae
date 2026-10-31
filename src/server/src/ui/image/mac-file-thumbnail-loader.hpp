#pragma once

#include <QFuture>
#include <QImage>
#include <QSize>
#include <QString>
#include <functional>

struct MacFileThumbnailRequest {
  QFuture<QImage> future;
  std::function<void()> cancel;
};

MacFileThumbnailRequest renderMacFileThumbnail(const QString &path, const QSize &size);
