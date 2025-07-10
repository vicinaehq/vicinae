#pragma once
#include <condition_variable>
#include <mutex>
#include <qobject.h>
#include <qthread.h>
#include "services/files-service/abstract-file-indexer.hpp"
#include <malloc.h>
#include <libqalculate/includes.h>
#include <qdatetime.h>
#include <qsqldatabase.h>
#include <qtmetamacros.h>
#include <queue>

class WriterWorker {
  QSqlDatabase db;
  std::mutex &batchMutex;
  std::deque<std::vector<std::filesystem::path>> &batchQueue;
  std::condition_variable &m_batchCv;

  void batchWrite(const std::vector<std::filesystem::path> &paths);

public:
  void run();

  WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
               std::condition_variable &batchCv)
      : batchMutex(batchMutex), batchQueue(batchQueue), m_batchCv(batchCv) {}
};

class QueryWorker {
public:
  void query(QString &id, const QString &query) {}
};

class IndexerScanner {
  static const size_t INDEX_BATCH_SIZE = 10000;

  std::deque<std::vector<std::filesystem::path>> m_writeBatches;
  std::mutex m_batchMutex;
  std::condition_variable m_batchCv;

  std::queue<std::filesystem::path> m_scanPaths;
  std::mutex m_scanMutex;
  std::condition_variable m_scanCv;

  std::thread m_writerThread;

  void scan(const std::filesystem::path &path);
  void enqueueBatch(const std::vector<std::filesystem::path> &paths);

public:
  void enqueue(const std::filesystem::path &path);
  void run();
};

class IndexerWorkerThread {
  std::thread m_scanner;
};

/**
 * Generic file indexer that should be usable in most environments.
 * SQLite FTS5 is used as a backend, making it technically possible to index the entire
 * filesystem if necessary.
 */
class FileIndexer : public AbstractFileIndexer {
  Q_OBJECT

  std::vector<Entrypoint> m_entrypoints;
  QSqlDatabase m_db;
  std::shared_ptr<IndexerScanner> m_scanner = std::make_shared<IndexerScanner>();
  std::thread m_scannerThread;

  enum ScanStatus {
    Started,
    Failed,
    Finished,
  };

  struct ScanRecord {
    int id;
    ScanStatus status;
    QDateTime createdAt;
  };

  // move that somewhere else later
  void createTables();

  ScanRecord createScan();
  std::vector<ScanRecord> listScans();

public:
  void setEntrypoints(const std::vector<Entrypoint> &entrypoints) override;
  std::vector<IndexerFileResult> query(std::string_view view) const override;
  virtual IndexerAsyncQuery *queryAsync(std::string_view view) const override;
  void start() override;

  FileIndexer();
};
