#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "utils.hpp"
#include <absl/strings/internal/str_format/extension.h>
#include <qobject.h>
#include <qthread.h>

class FileIndexerQueryEngineWorker : public QObject {
  Q_OBJECT

signals:
  void queryFinished(std::vector<IndexerFileResult>, const QString &query) const;

public:
  FileIndexerQueryEngineWorker(QObject *parent = nullptr) : QObject(parent) {
    db = new FileIndexerDatabase;
    db->setParent(this);
  }

public slots:
  void handleQuery(const QString &query, const AbstractFileIndexer::QueryParams &params) {
    std::vector<std::filesystem::path> paths;
    QString finalQuery = preparePrefixSearchQuery(query);
    paths = db->search(finalQuery.toStdString(), params);

    std::vector<IndexerFileResult> results;

    for (const auto &path : paths) {
      results.emplace_back(IndexerFileResult{.path = path});
    }

    emit queryFinished(results, query);
  }

private:
  QString preparePrefixSearchQuery(QStringView query) const {
    QString finalQuery;

    for (const auto &word : query.split(' ')) {
      if (!finalQuery.isEmpty()) { finalQuery += ' '; }
      finalQuery += QString("\"%1\"").arg(word);
    }

    finalQuery += '*';

    return finalQuery;
  }

  FileIndexerDatabase *db;
};

class FileIndexerQueryEngine : public QObject {
  Q_OBJECT

signals:
  void queryRequested(const QString &query, const AbstractFileIndexer::QueryParams &params) const;

public:
  FileIndexerQueryEngine() {
    auto worker = new FileIndexerQueryEngineWorker;
    worker->moveToThread(&m_workerThread);

    connect(this, &FileIndexerQueryEngine::queryRequested, worker,
            &FileIndexerQueryEngineWorker::handleQuery);
    connect(worker, &FileIndexerQueryEngineWorker::queryFinished, this,
            &FileIndexerQueryEngine::handleFinished);
    m_workerThread.start();
  }

  ~FileIndexerQueryEngine() {
    m_workerThread.quit();
    m_workerThread.wait();
  }

  QFuture<std::vector<IndexerFileResult>> query(std::string_view q,
                                                const AbstractFileIndexer::QueryParams &params) {
    QString str = qStringFromStdView(q);
    m_currentQuery = str;
    m_promise = std::make_unique<QPromise<std::vector<IndexerFileResult>>>();
    emit queryRequested(str, params);
    return m_promise->future();
  }

private:
  void handleFinished(std::vector<IndexerFileResult> files, const QString &query) {
    if (m_currentQuery != query) return;
    m_promise->addResult(files);
    m_promise->finish();
  }

  std::unique_ptr<QPromise<std::vector<IndexerFileResult>>> m_promise;
  QString m_currentQuery;
  QThread m_workerThread;
};
