#pragma once
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qsqlquery.h>
#include <qthread.h>
#include "common.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/indexer-scanner.hpp"
#include "services/files-service/file-indexer/home-directory-watcher.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <malloc.h>
#include <qdatetime.h>
#include <qsqldatabase.h>
#include <qtmetamacros.h>

class WriterWorker : public NonCopyable {
  std::unique_ptr<FileIndexerDatabase> db;
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
  FileIndexerDatabase m_db;
  std::shared_ptr<IndexerScanner> m_scanner = std::make_shared<IndexerScanner>();
  std::thread m_scannerThread;
  std::unique_ptr<HomeDirectoryWatcher> m_homeWatcher;

  // move that somewhere else later
  QString preparePrefixSearchQuery(std::string_view query) const;

public:
  void startFullscan();
  void rebuildIndex() override;
  void setEntrypoints(const std::vector<Entrypoint> &entrypoints) override;
  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                     const QueryParams &params = {}) const override;
  void start() override;

  FileIndexer();
  ~FileIndexer();
};
