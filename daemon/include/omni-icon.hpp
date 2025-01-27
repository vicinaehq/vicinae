#pragma once

#include "image-fetcher.hpp"
#include <qlabel.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qtmetamacros.h>
#include "favicon-fetcher.hpp"

class OmniIcon : public QLabel {
  Q_OBJECT

  QString currentId;

  void setDefaultIcon(QSize size) { setPixmap(QIcon::fromTheme("application-x-executable").pixmap(size)); }
  void setPixmap(const QPixmap &pixmap) {
    qDebug() << "set pixmap!!!";
    emit imageUpdated(pixmap);
    QLabel::setPixmap(pixmap);
  }

public:
  OmniIcon() {}
  ~OmniIcon() {}

  void setIcon(const QString &id, QSize size, const QString &fallback = "") {
    currentId = id;

    if (size.isValid()) setFixedSize(size);

    auto ss = id.split(":");
    QString type, name;

    if (ss.size() == 2) {
      type = ss.at(0);

      if (type.isEmpty()) {
        name = ":" + ss.at(1);
      } else {
        name = ss.at(1);
      }
    } else {
      name = ss.at(0);
    }

    qDebug() << "type" << type << "name" << name;

    if (type == "favicon") {
      QPixmap pm;

      if (QPixmapCache::find(name, &pm)) {
        setPixmap(pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        return;
      }

      auto reply = FaviconFetcher::fetchFavicon(name, size);

      connect(reply, &FaviconReply::failed, this, [this, fallback, size, reply]() {
        if (!fallback.isEmpty()) {
          setIcon(fallback, size);
        } else {
          setDefaultIcon(size);
        }

        reply->deleteLater();
      });

      connect(reply, &FaviconReply::finished, this, [this, size, reply](QPixmap pixmap) {
        setPixmap(pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        reply->deleteLater();
      });

      return;
    }

    if (type == "url") {
      QUrl url(name);

      if (url.isValid()) {
        auto reply = ImageFetcher::instance().fetch(url.toString(), {});

        connect(reply, &ImageReply::imageLoaded, this,
                [this, size](QPixmap pixmap) { setPixmap(pixmap.scaled(size)); });
      }

      return;
    }

    auto icon = QIcon::fromTheme(name);

    if (icon.isNull()) {
      if (fallback.isEmpty())
        setDefaultIcon(size);
      else
        return setIcon(fallback, size);
    }

    setPixmap(icon.pixmap(size));
  }

signals:
  void imageUpdated(QPixmap pixmap);
};
