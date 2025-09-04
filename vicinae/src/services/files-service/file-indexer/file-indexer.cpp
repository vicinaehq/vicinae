#include <cstdlib>
#include <filesystem>
#include <memory>
#include <mutex>
#include <QtConcurrent/QtConcurrent>
#include "services/files-service/abstract-file-indexer.hpp"
#include "services/files-service/file-indexer/indexer-scanner.hpp"
#include "services/files-service/file-indexer/incremental-scanner.hpp"
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

void FileIndexer::startFullscan() {
  for (const auto &entrypoint : m_entrypoints) {
    m_dispatcher.enqueue({.type = ScanType::Full, .path = entrypoint.root});
  }
}

void FileIndexer::rebuildIndex() { startFullscan(); }

void FileIndexer::start() {
  m_dispatcher.enableAll();

  auto lastScan = m_db.getLastScan();

  // this is our first scan
  if (!lastScan) {
    qInfo() << "This is our first startup, enqueuing a full scan...";
    for (const auto &entrypoint : m_entrypoints) {
      m_dispatcher.enqueue({.type = ScanType::Full, .path = entrypoint.root});
    }
    return;
  }

  // Scans marked as started when we call start() (that is, at the beginning of the program)
  // are considered failed because they were not able to finish.
  auto startedScans = m_db.listStartedScans();

  for (const auto &scan : startedScans) {
    qWarning() << "Creating new scan after previous scan for" << scan.path.c_str() << "was interrupted";
    m_db.setScanError(scan.id, "Interrupted");
    m_dispatcher.enqueue({.type = scan.type, .path = scan.path});
  }

  if (startedScans.empty()) {
    for (const auto &entrypoint : m_entrypoints) {
      m_dispatcher.enqueue({.type = ScanType::Incremental, .path = entrypoint.root, .maxDepth = 5});
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

FileIndexer::FileIndexer()
    : m_dispatcher({{ScanType::Full, std::make_shared<IndexerScanner>()},
                    {ScanType::Incremental, std::make_shared<IncrementalScanner>()}}) {

  m_db.runMigrations();
}
