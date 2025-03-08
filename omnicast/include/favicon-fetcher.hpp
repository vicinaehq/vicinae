#pragma once
#include "image-fetcher.hpp"
#include <qobject.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qtmetamacros.h>

class FaviconReply : public QObject {
  Q_OBJECT

public:
  static FaviconReply *instantReply(QPixmap pixmap) {
    auto reply = new FaviconReply();

    QTimer::singleShot(0, [reply, pixmap]() { emit reply->finished(pixmap); });

    return reply;
  }

signals:
  void finished(QPixmap pixmap);
  void failed();
};

class FaviconFetcher {
public:
  static ImageReply *fetch(const QString &domain, QSize size) {
    auto serializedSize = QString("%1x%2").arg(size.width()).arg(size.height());
    auto url = QString("https://icons.duckduckgo.com/ip3/%1.ico").arg(domain);

    QPixmap cached;

    QPixmapCache::find(domain, &cached);

    qDebug() << "fetch" << url;

    return ImageFetcher::instance().fetch(url, {});
  }

  static FaviconReply *fetchFavicon(const QString &domain, QSize size) {
    auto serializedSize = QString("%1x%2").arg(size.width()).arg(size.height());
    auto url = QString("https://www.google.com/s2/favicons?domain=%1&sz=64").arg(domain);

    QPixmap pm;

    if (QPixmapCache::find(domain, &pm)) {
      qDebug() << "cached favicon";
      return FaviconReply::instantReply(pm);
    }

    qDebug() << "fetch" << url;

    auto reply = ImageFetcher::instance().fetch(url, {});
    auto faviconReply = new FaviconReply();

    QObject::connect(reply, &ImageReply::loadingError, faviconReply, [reply, faviconReply]() {
      emit faviconReply->failed();
      reply->deleteLater();
    });
    QObject::connect(reply, &ImageReply::imageLoaded, faviconReply,
                     [domain, faviconReply, reply](QPixmap pixmap) {
                       QPixmapCache::insert(domain, pixmap);
                       emit faviconReply->finished(pixmap);
                       reply->deleteLater();
                     });

    return faviconReply;
  }
};
