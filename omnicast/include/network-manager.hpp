#pragma once
#include <deque>
#include <qnetworkaccessmanager.h>
#include <qnetworkdiskcache.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qstandardpaths.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qurl.h>

class ImageNetworkReply : public QObject {
  Q_OBJECT
  QUrl m_url;
  QNetworkReply *m_reply = nullptr;

public:
  ImageNetworkReply(const QUrl &url) : m_url(url) {}

  void setReply(QNetworkReply *reply) {
    m_reply = reply;
    connect(m_reply, &QNetworkReply::requestSent, this, &ImageNetworkReply::requestSent);
    connect(m_reply, &QNetworkReply::finished, this, &ImageNetworkReply::finished);
  }
  QNetworkReply *reply() const { return m_reply; }

  const QUrl &url() const { return m_url; }

  QByteArray readAll() {
    if (m_reply) return m_reply->readAll();
    return {};
  }

  void abort() {
    if (m_reply) m_reply->abort();
    emit aborted();
  }

signals:
  void requestSent() const;
  void finished();
  void aborted();
};

class QueuedNetworkManager : public QObject {
  size_t MAX_CONCURRENT = 1;

  std::deque<ImageNetworkReply *> m_requests;
  std::vector<QNetworkReply *> m_replies;
  QNetworkAccessManager *m_manager = new QNetworkAccessManager(this);

  void handleFinished(QNetworkReply *reply) {
    if (auto it = std::ranges::find(m_replies, reply); it != m_replies.end()) { m_replies.erase(it); }
    reply->deleteLater();
    enqueueNext();
  }

  void enqueueNext() {
    while (m_replies.size() < MAX_CONCURRENT && !m_requests.empty()) {
      ImageNetworkReply *request = m_requests.front();
      m_requests.pop_front();
      enqueue(request);
    }
  }

  void enqueue(ImageNetworkReply *reply) {
    auto nreply = m_manager->get(QNetworkRequest(reply->url()));

    reply->setReply(nreply);
    m_replies.emplace_back(nreply);
    connect(nreply, &QNetworkReply::finished, this, [this, nreply]() { handleFinished(nreply); });
  }

public:
  ImageNetworkReply *get(const QUrl &url) {
    auto request = new ImageNetworkReply(url);

    connect(request, &ImageNetworkReply::aborted, this, [this, request]() {
      if (auto it = std::ranges::find(m_requests, request); it != m_requests.end()) { m_requests.erase(it); }
      if (auto reply = request->reply()) { handleFinished(reply); }
    });

    m_requests.push_back(request);
    enqueueNext();
    return request;
  }

  static QueuedNetworkManager *instance() {
    static QueuedNetworkManager instance;

    return &instance;
  }
};

class NetworkManager {
  QNetworkAccessManager *m_manager = new QNetworkAccessManager;
  QNetworkDiskCache *m_diskCache = new QNetworkDiskCache;

public:
  NetworkManager() {
    QString directory =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1StringView("/omnicast/");
    qDebug() << "cache dir" << directory;
    m_diskCache->setCacheDirectory(directory);
    m_manager->setCache(m_diskCache);
  }

  QNetworkAccessManager *manager() { return m_manager; }

  static NetworkManager *instance() {
    static NetworkManager instance;

    return &instance;
  }
};
