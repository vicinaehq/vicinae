#pragma once
#include "image-fetcher.hpp"
#include <qobject.h>
#include <qpixmap.h>
#include <qpixmapcache.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qwidget.h>

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
    auto placeholderUrl = QString("https://www.google.com/s2/favicons?domain=%1&sz=%2");
    std::vector<uint> sizes = {128, 64, 32, 16};

    QPixmap pm;

    if (QPixmapCache::find(domain, &pm)) {
      qDebug() << "cached favicon";
      return FaviconReply::instantReply(pm);
    }

    auto reply = ImageFetcher::instance().fetch(placeholderUrl.arg(domain).arg(64), {});
    auto faviconReply = new FaviconReply();

    QObject::connect(reply, &ImageReply::loadingError, faviconReply, [reply, faviconReply]() {
      reply->deleteLater();
      emit faviconReply->failed();
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

class FaviconRequest : public QObject {
  Q_OBJECT
  QString _domain;

public:
  QString domain() const { return _domain; }
  FaviconRequest(const QString &domain, QObject *parent = nullptr) : QObject(parent), _domain(domain) {}
  virtual void start() = 0;

signals:
  void finished(QPixmap favicon) const;
  void failed() const;
};

class DummyFaviconRequest : public FaviconRequest {
public:
  DummyFaviconRequest(const QString &domain, QObject *parent = nullptr) : FaviconRequest(domain, parent) {
    QTimer::singleShot(0, this, &DummyFaviconRequest::failed);
  }

  void start() override {
    qDebug() << "Started DummyFaviconRequest!";
    emit failed();
  }
};

class CachedFaviconRequest : public FaviconRequest {
  QPixmap _data;

  void start() override { emit finished(_data); }

public:
  CachedFaviconRequest(const QString &domain, const QPixmap &data, QObject *parent = nullptr)
      : FaviconRequest(domain, parent), _data(data) {}
};

class GoogleFaviconRequester : public FaviconRequest {
  const std::vector<uint> sizes = {128, 64, 32, 16};
  size_t currentSizeAttemptIndex = 0;
  QString placeholderUrl;
  ImageReply *_currentReply;

  QString makeUrl(uint size) const { return placeholderUrl.arg(domain()).arg(size); }

  void loadingFailed() {
    _currentReply->deleteLater();
    _currentReply = nullptr;
    currentSizeAttemptIndex += 1;
    if (currentSizeAttemptIndex > sizes.size()) {
      emit loadingFailed();
      return;
    }

    tryForCurrentSize();
  }

  void imageLoaded(QPixmap pixmap) {
    _currentReply->deleteLater();
    _currentReply = nullptr;
    emit finished(pixmap);
  }

  void tryForCurrentSize() {
    if (currentSizeAttemptIndex >= sizes.size()) { return; }

    auto serviceUrl = makeUrl(sizes.at(currentSizeAttemptIndex));
    auto reply = ImageFetcher::instance().fetch(serviceUrl, {});

    connect(reply, &ImageReply::imageLoaded, this, &GoogleFaviconRequester::imageLoaded);
    connect(reply, &ImageReply::loadingError, this, &GoogleFaviconRequester::loadingFailed);
    _currentReply = reply;
  }

public:
  GoogleFaviconRequester(const QString &domain, QObject *parent = nullptr)
      : FaviconRequest(domain, parent), placeholderUrl("https://www.google.com/s2/favicons?domain=%1&sz=%2"),
        _currentReply(nullptr) {}
  ~GoogleFaviconRequester() {
    if (_currentReply) { _currentReply->deleteLater(); }
  }

  void start() override {
    currentSizeAttemptIndex = 0;

    if (_currentReply) {
      _currentReply->deleteLater();
      _currentReply = nullptr;
    }

    tryForCurrentSize();
  }
};
