#include <cstdlib>
#include <filesystem>
#include <mutex>
#include <QtConcurrent/QtConcurrent>
#include "services/files-service/abstract-file-indexer.hpp"
#include "file-indexer-db.hpp"
#include "file-indexer.hpp"
#include "utils/utils.hpp"
#include <QDebug>
#include <qcryptographichash.h>
#include <qfilesystemwatcher.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <QSqlError>
#include <qthreadpool.h>
#include <ranges>
#include <thread>
#include <unistd.h>

namespace fs = std::filesystem;

/*
 * List of absolute paths to never follow during indexing. Mostly used to exclude
 * pseudo filesystems such as /run or /proc.
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::vector<fs::path> EXCLUDED_PATHS = {"/sys", "/run",     "/proc", "/tmp",

                                                     "/mnt", "/var/tmp", "/efi",  "/dev"};

void WriterWorker::stop() {
  m_alive = false;
  m_batchCv.notify_one();
}

void WriterWorker::run() {
  db = std::make_unique<FileIndexerDatabase>();

  while (m_alive) {
    std::deque<std::vector<std::filesystem::path>> batch;

    {
      std::unique_lock<std::mutex> lock(batchMutex);

      m_batchCv.wait(lock, [&]() { return !batchQueue.empty(); });
      batch = std::move(batchQueue);
      batchQueue.clear();
    }

    for (const auto &paths : batch) {
      batchWrite(paths);
    }
  }
}

void WriterWorker::batchWrite(const std::vector<fs::path> &paths) {
  // Writing is happening in the writerThread
  db->indexFiles(paths);
  qCritical() << "INDEXED" << paths.size() << "files";
}

WriterWorker::WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
                           std::condition_variable &batchCv)
    : batchMutex(batchMutex), batchQueue(batchQueue), m_batchCv(batchCv) {}

void FileIndexer::start() {
  auto lastScan = m_db.getLastScan();

  if (!lastScan) {
    m_db.createScan();

    for (const auto &entrypoint : m_entrypoints) {
      m_scanner->enqueue(entrypoint.root, IndexerScanner::EnqueuedScan::Full);
    }
    return;
  }

  for (const auto &entrypoint : m_entrypoints) {
    // if last scan is not old enough we limit the max depth to perform a faster scan
    m_scanner->enqueue(entrypoint.root, IndexerScanner::EnqueuedScan::Incremental, 5);
  }
}

void FileIndexer::setEntrypoints(const std::vector<Entrypoint> &entrypoints) { m_entrypoints = entrypoints; }

QString FileIndexer::preparePrefixSearchQuery(std::string_view query) const {
  QString finalQuery;

  for (const auto &word : std::views::split(query, std::string_view(" "))) {
    std::string_view view(word);

    if (!finalQuery.isEmpty()) { finalQuery += ' '; }

    finalQuery += QString("\"%1\"").arg(qStringFromStdView(view));
  }

  finalQuery += '*';

  return finalQuery;
}

IndexerAsyncQuery *FileIndexer::queryAsync(std::string_view view, const QueryParams &params) const {
  auto asyncQuery = new IndexerAsyncQuery();
  auto searchQuery = qStringFromStdView(view);
  QString finalQuery = preparePrefixSearchQuery(view);

  QThreadPool::globalInstance()->start([asyncQuery, params, finalQuery]() {
    std::vector<fs::path> paths;
    {
      FileIndexerDatabase db;
      paths = db.search(finalQuery.toStdString(), params);
    }
    std::vector<IndexerFileResult> results =
        paths | std::views::transform([](auto &&path) { return IndexerFileResult{.path = path}; }) |
        std::ranges::to<std::vector>();

    emit asyncQuery->finished(results);
  });

  return asyncQuery;
}

FileIndexer::FileIndexer() {
  m_db.runMigrations();
  m_homeWatcher = std::make_unique<HomeDirectoryWatcher>(*m_scanner.get());
  m_scannerThread = std::thread([&]() { m_scanner->run(); });
}

FileIndexer::~FileIndexer() {
  m_scanner->stop();
  m_scannerThread.join();
}
