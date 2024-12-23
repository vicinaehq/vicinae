#include "remote-image-viewer.hpp"
#include "image-fetcher.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qpixmapcache.h>
#include <qsize.h>

static QString makeSizedCacheKey(const QString &url, QSize size) {
  return url + QString::number(size.width()) + QString::number(size.height());
}

RemoteImageViewer::RemoteImageViewer(const QString &url, Qt::Alignment align,
                                     QSize size)
    : label(new QLabel), url(url), align(align), scaled(size) {
  auto layout = new QVBoxLayout();

  layout->addWidget(label);
  layout->setContentsMargins(0, 0, 0, 0);

  setLayout(layout);

  if (size.isValid()) {
    label->setFixedSize(size);
    const QString &cacheKey = makeSizedCacheKey(url, size);
    QPixmap pix;

    if (QPixmapCache::find(cacheKey, &pix)) {
      qDebug() << "cached in QPixmapCache!";
      loaded(pix);
      return;
    }
  }

  auto reply = ImageFetcher::instance().fetch(url, {.cache = false});

  connect(reply, &ImageReply::imageLoaded, this, &RemoteImageViewer::loaded);
}

void RemoteImageViewer::loaded(QPixmap pix) {
  int width = label->width();
  int height = label->height();

  if (scaled.isValid()) {
    width = scaled.width();
    height = scaled.height();
  }

  auto scaledPix =
      pix.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  label->setAlignment(Qt::AlignCenter);
  label->setPixmap(scaledPix);

  if (scaled.isValid() && scaled.width() * scaled.height() < maxCacheSize) {
    auto key = makeSizedCacheKey(url, scaled);

    QPixmapCache::insert(key, scaledPix);
  }

  emit imageLoaded();
}

RemoteImageViewer::~RemoteImageViewer() {
  qDebug() << "delete remote image viewer";
}
