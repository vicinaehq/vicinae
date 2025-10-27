#include "file-indexer.hpp"

#include <filesystem>
#include <QtConcurrent/QtConcurrent>
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "file-indexer-db.hpp"
#include "utils/utils.hpp"
#include <QDebug>
#include <ranges>
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
  for (auto const &[id, scan] : m_dispatcher.scans()) {
    if (scan.type == ScanType::Full || scan.type == ScanType::Watcher) { m_dispatcher.interrupt(id); }
  }

  for (const auto &entrypoint : m_entrypoints) {
    m_dispatcher.enqueue({.type = ScanType::Full, .path = entrypoint});
  }

  for (const auto &entrypoint : m_watcherPaths) {
    m_dispatcher.enqueue({.type = ScanType::Watcher, .path = entrypoint});
  }
}

void FileIndexer::rebuildIndex() { startFullscan(); }

void FileIndexer::start() {
  auto lastScan = m_db.getLastScan();

  // this is our first scan
  if (!lastScan) {
    qInfo() << "This is our first startup, enqueuing a full scan...";
    startFullscan();
    return;
  }

  // Scans marked as started when we call start() (that is, at the beginning of the program)
  // are considered failed because they were not able to finish.
  auto startedScans = m_db.listStartedScans();

  for (const auto &scan : startedScans) {
    if (scan.type != ScanType::Full) continue;
    qWarning() << "Creating new scan after previous scan for" << scan.path.c_str() << "was interrupted";
    m_writer->setScanError(scan.id, "Interrupted");
    m_dispatcher.enqueue({.type = scan.type, .path = scan.path});
  }

  if (startedScans.empty()) {
    for (const auto &entrypoint : m_entrypoints) {
      m_dispatcher.enqueue({.type = ScanType::Incremental, .path = entrypoint, .maxDepth = 5});
    }
  }

  for (const auto &entrypoint : m_watcherPaths) {
    m_dispatcher.enqueue(
        {.type = ScanType::Watcher, .path = entrypoint, .excludedFilenames = m_excludedFilenames});
  }
}

QString FileIndexer::preparePrefixSearchQuery(std::string_view query) const {
  QString finalQuery;

  for (const auto &word : std::views::split(query, std::string_view(" "))) {
    std::string_view view(word.begin(), word.end());

    if (!finalQuery.isEmpty()) { finalQuery += ' '; }

    finalQuery += QString("\"%1\"").arg(qStringFromStdView(view));
  }

  finalQuery += '*';

  return finalQuery;
}

void FileIndexer::preferenceValuesChanged(const QJsonObject &preferences) {
  m_entrypoints = ranges_to<std::vector>(
      preferences.value("paths").toString().split(';', Qt::SkipEmptyParts) |
      std::views::transform([](const QStringView &v) { return fs::path(v.toString().toStdString()); }));

  m_watcherPaths = ranges_to<std::vector>(
      preferences.value("watcherPaths").toString().split(';', Qt::SkipEmptyParts) |
      std::views::transform([](const QStringView &v) { return fs::path(v.toString().toStdString()); }));

  std::string databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};
}

QFuture<std::vector<IndexerFileResult>> FileIndexer::queryAsync(std::string_view view,
                                                                const QueryParams &params) const {
  auto searchQuery = qStringFromStdView(view);
  QString finalQuery = preparePrefixSearchQuery(view);
  auto promise = std::make_shared<QPromise<std::vector<IndexerFileResult>>>();
  auto future = promise->future();

  QThreadPool::globalInstance()->start([params, finalQuery, promise = std::move(promise)]() mutable {
    std::vector<fs::path> paths;
    {
      FileIndexerDatabase db;
      paths = db.search(finalQuery.toStdString(), params);
    }

    std::vector<IndexerFileResult> results;

    for (const auto &path : paths) {
      results.emplace_back(IndexerFileResult{.path = path});
    }

    promise->addResult(results);
    promise->finish();
  });

  return future;
}

FileIndexer::FileIndexer() : m_writer(std::make_shared<DbWriter>()), m_dispatcher(m_writer) {
  m_db.runMigrations();
}
