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

void FileIndexer::startFullscan() {
  for (const auto &entrypoint : m_entrypoints) {
    m_scanner->enqueueFull(entrypoint.root);
  }
}

void FileIndexer::rebuildIndex() { startFullscan(); }

void FileIndexer::start() {
  auto lastScan = m_db.getLastScan();

  // this is our first scan
  if (!lastScan) {
    qInfo() << "This is our first startup, enqueuing a full scan...";
    for (const auto &entrypoint : m_entrypoints) {
      m_scanner->enqueueFull(entrypoint.root);
    }
    return;
  }

  // Scans marked as started when we call start() (that is, at the beginning of the program)
  // are considered failed because they were not able to finish.
  auto startedScans = m_db.listStartedScans();

  for (const auto &scan : startedScans) {
    qCritical() << "Creating new scann after previous scan for" << scan.path << "failed";
    m_db.setScanError(scan.id, "Interrupted");
    m_scanner->enqueue(scan.path, scan.type);
  }

  if (startedScans.empty()) {
    for (const auto &entrypoint : m_entrypoints) {
      m_scanner->enqueue(entrypoint.root, FileIndexerDatabase::ScanType::Incremental, 5);
    }
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

QFuture<std::vector<IndexerFileResult>> FileIndexer::queryAsync(std::string_view view,
                                                                const QueryParams &params) const {
  auto searchQuery = qStringFromStdView(view);
  QString finalQuery = preparePrefixSearchQuery(view);
  QPromise<std::vector<IndexerFileResult>> promise;
  auto future = promise.future();

  QThreadPool::globalInstance()->start([params, finalQuery, promise = std::move(promise)]() mutable {
    std::vector<fs::path> paths;
    {
      FileIndexerDatabase db;
      paths = db.search(finalQuery.toStdString(), params);
    }
    std::vector<IndexerFileResult> results =
        paths | std::views::transform([](auto &&path) { return IndexerFileResult{.path = path}; }) |
        std::ranges::to<std::vector>();

    promise.addResult(results);
    promise.finish();
  });

  return future;
}

FileIndexer::FileIndexer() {
  m_db.runMigrations();
  // m_homeWatcher = std::make_unique<HomeDirectoryWatcher>(*m_scanner.get());
  m_scannerThread = std::thread([&]() { m_scanner->run(); });
}

FileIndexer::~FileIndexer() {
  m_scanner->stop();
  m_scannerThread.join();
}
