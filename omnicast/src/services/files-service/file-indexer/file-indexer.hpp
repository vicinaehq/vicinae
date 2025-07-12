#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <qobject.h>
#include <qsqlquery.h>
#include <qthread.h>
#include "common.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include <malloc.h>
#include <libqalculate/includes.h>
#include <qdatetime.h>
#include <qsqldatabase.h>
#include <qtmetamacros.h>
#include <queue>

class WriterWorker : public NonCopyable {
  QSqlDatabase db;
  std::mutex &batchMutex;
  std::deque<std::vector<std::filesystem::path>> &batchQueue;
  std::condition_variable &m_batchCv;
  std::atomic<bool> m_alive = true;

  void batchWrite(const std::vector<std::filesystem::path> &paths);

public:
  void run();
  void stop();

  WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
               std::condition_variable &batchCv);
};

class IncrementalScanner : public NonCopyable {
  QSqlDatabase &m_db;

  std::vector<std::filesystem::path> getScannableDirectories(const std::filesystem::path &path,
                                                             std::optional<size_t> maxDepth) const;
  void processDirectory(const std::filesystem::path &path);
  std::optional<QDateTime> retrieveIndexedLastModified(const std::filesystem::path &path) const;
  std::vector<std::filesystem::path> listIndexedDirectoryFiles(const std::filesystem::path &path) const;

public:
  void scan(const std::filesystem::path &path, std::optional<size_t> maxDepth);
  IncrementalScanner(QSqlDatabase &db);
};

class IndexerScanner : public NonCopyable {
  static constexpr size_t INDEX_BATCH_SIZE = 10'000;
  static constexpr size_t MAX_PENDING_BATCH_COUNT = 10;
  static constexpr size_t BACKPRESSURE_WAIT_MS = 100;

  std::atomic<bool> m_alive = true;
  std::deque<std::vector<std::filesystem::path>> m_writeBatches;
  std::mutex m_batchMutex;
  std::condition_variable m_batchCv;

  std::queue<std::filesystem::path> m_scanPaths;
  std::mutex m_scanMutex;
  std::condition_variable m_scanCv;

  std::unique_ptr<WriterWorker> m_writerWorker;
  std::thread m_writerThread;

  void scan(const std::filesystem::path &path);
  void enqueueBatch(const std::vector<std::filesystem::path> &paths);

public:
  void enqueue(const std::filesystem::path &path);
  void run();
  void stop();
};

/**
 * Generic file indexer that should be usable in most linux environments.
 * SQLite FTS5 is used as a backend, making it technically possible to index the entire
 * filesystem if necessary.
 * Queries usually remain very fast (<100ms), although not fast enough to perform them in the UI thread
 * without introducing slowdowns.
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
  std::optional<ScanRecord> getLastScan() const;
  std::vector<ScanRecord> listScans();
  ScanRecord mapScan(const QSqlQuery &query) const;

  QString preparePrefixSearchQuery(std::string_view query) const;

public:
  void setEntrypoints(const std::vector<Entrypoint> &entrypoints) override;
  virtual IndexerAsyncQuery *queryAsync(std::string_view view, const QueryParams &params) const override;
  void start() override;

  FileIndexer();
  ~FileIndexer();
};
