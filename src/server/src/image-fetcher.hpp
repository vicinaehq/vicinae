#pragma once
#include "vicinae.hpp"
#include <qmetacontainer.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkdiskcache.h>
#include <qnetworkreply.h>
#include <qobject.h>
#include <qstandardpaths.h>
#include <qstringview.h>
#include <qthread.h>
#include <deque>
#include <qtmetamacros.h>
#include <qurl.h>
#include <unordered_map>
#include "common/qt.hpp"
#include <QUuid>

class FetcherWorker : public QObject {
  Q_OBJECT

  std::unordered_map<QString, QObjectUniquePtr<QNetworkReply>> m_replies;

  QNetworkAccessManager *m_manager = nullptr;
  QNetworkDiskCache *m_diskCache = nullptr;

  void handleFetchRequest(const QString &id, const QUrl &url) {
    if (!m_manager) return;

    QNetworkRequest req(url);

    req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    auto reply = m_manager->get(req);

    connect(reply, &QNetworkReply::finished, this, [this, id, reply]() {
      emit fetchFinished(id, reply->readAll());
      m_replies.erase(id);
    });

    m_replies.insert({id, QObjectUniquePtr<QNetworkReply>(reply)});
  }

public:
  void initialize() {
    QString directory =
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QString("/vicinae/");

    m_manager = new QNetworkAccessManager;
    m_diskCache = new QNetworkDiskCache;
    m_diskCache->setCacheDirectory(directory);
    m_diskCache->setMaximumCacheSize(Omnicast::IMAGE_DISK_CACHE_MAX_SIZE);
    m_manager->setCache(m_diskCache);

    connect(this, &FetcherWorker::fetchRequested, this, &FetcherWorker::handleFetchRequest);
  }

signals:
  void fetchRequested(const QString &id, const QUrl &url);
  void abortRequested(const QString &id);
  void fetchFinished(const QString &id, const QByteArray &array);
};

class FetchReply : public QObject {
  Q_OBJECT

  QUrl m_url;

public:
  const QUrl &url() const { return m_url; }
  void abort() { emit aborted(); }

  FetchReply(const QUrl &url) : m_url(url) {}

signals:
  void finished(const QByteArray &data) const;
  void aborted() const;
};

class NetworkFetcher : public QObject {
  Q_OBJECT

  size_t m_concurrency = 6;
  FetcherWorker *m_worker = new FetcherWorker;
  QThread *m_thread = new QThread;

  struct QueuedReply {
    QString id;
    FetchReply *reply;
  };

  std::unordered_map<QString, FetchReply *> m_replies;
  std::deque<QueuedReply> m_queue;

  void handleFetchFinished(const QString &id, const QByteArray &data) {
    if (auto it = m_replies.find(id); it != m_replies.end()) {
      emit it->second->finished(data);
      m_replies.erase(id);
    }
    startRequests();
  }

  void startRequests() {
    while (!m_queue.empty() && m_replies.size() < m_concurrency) {
      auto queued = m_queue.back();
      QString id = queued.id;
      m_queue.pop_back();

      emit fetchRequested(id, queued.reply->url());
      m_replies.insert({id, queued.reply});
    }
  }

public:
  FetchReply *fetch(const QUrl &url) {
    auto id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    auto reply = new FetchReply(url);

    connect(reply, &FetchReply::aborted, this, [this, reply, id]() {
      if (auto it = std::ranges::find_if(m_queue, [&](auto &&data) { return data.reply == reply; });
          it != m_queue.end()) {
        m_queue.erase(it);
      };

      m_replies.erase(id);
      emit abortRequested(id);
    });

    m_queue.emplace_back(QueuedReply{id, reply});
    startRequests();

    return reply;
  }

  NetworkFetcher() {
    connect(this, &NetworkFetcher::fetchRequested, m_worker, &FetcherWorker::fetchRequested);
    connect(m_thread, &QThread::started, m_worker, &FetcherWorker::initialize);
    connect(m_worker, &FetcherWorker::fetchFinished, this, &NetworkFetcher::handleFetchFinished);
    connect(m_worker, &FetcherWorker::abortRequested, this, &NetworkFetcher::abortRequested);
    m_worker->moveToThread(m_thread);
    m_thread->start();
  }

  static NetworkFetcher *instance() {
    static NetworkFetcher instance;

    return &instance;
  }

signals:
  void fetchRequested(const QString &id, const QUrl &url) const;
  void abortRequested(const QString &id);
};
