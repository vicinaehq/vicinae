#pragma once

#include "common.hpp"
#include <qlogging.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkdiskcache.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstandardpaths.h>
#include <qtmetamacros.h>

struct FetchImageOptions {
  bool cache;
};

class ImageReply : public QObject {
  Q_OBJECT
  QNetworkReply *reply;

  void finished() {
    if (reply->error() != QNetworkReply::NoError) {
      emit loadingError();
      return;
    }

    auto buf = reply->readAll();
    QPixmap pix;

    pix.loadFromData(buf);
    emit imageLoaded(pix);
  }

public:
  ImageReply(QNetworkReply *reply) : reply(reply) {
    connect(reply, &QNetworkReply::finished, this, &ImageReply::finished);
  }

  ~ImageReply() { delete reply; }

signals:
  void imageLoaded(QPixmap img);
  void loadingError();
};

class ImageFetcher : public NonAssignable {
  QNetworkAccessManager *manager;
  QNetworkDiskCache *diskCache;

public:
  static const ImageFetcher &instance() {
    static ImageFetcher fetcher;

    return fetcher;
  }

  ImageFetcher() : manager(new QNetworkAccessManager), diskCache(new QNetworkDiskCache) {
    QString directory =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1StringView("/omnicast/");
    qDebug() << "cache dir" << directory;
    manager->setCache(diskCache);
  }

  ImageReply *fetch(const QString &url, const FetchImageOptions &options = {.cache = true}) const {
    QNetworkRequest request(url);

    if (options.cache) {
      request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    } else {
      request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    }

    return new ImageReply(manager->get(request));
  }

  ~ImageFetcher() {
    qDebug() << "remove fetcher";
    delete manager;
  }
};
