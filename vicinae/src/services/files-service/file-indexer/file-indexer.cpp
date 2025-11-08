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
#include <thread>

namespace fs = std::filesystem;

/*
 * List of absolute paths to never follow during indexing. Mostly used to exclude
 * pseudo filesystems such as /run or /proc.
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::vector<fs::path> EXCLUDED_PATHS = {"/sys", "/run",     "/proc", "/tmp",
                                                     "/mnt", "/var/tmp", "/efi",  "/dev"};

void FileIndexer::startFullScan() {
  m_dispatcher.interruptAll();

  // Prevent starting the full scan and watchers before deletion is done
  std::thread([this]() {
    qInfo() << "Starting full scan, clearing existing index...";

    // Delete all indexed files, then enqueue new scans in the completion callback
    m_writer->deleteAllIndexedFiles([this]() {
      qInfo() << "Existing index cleared, enqueuing full scan tasks...";

      for (const auto &entrypoint : m_entrypoints) {
        qInfo() << "Enqueuing full scan for" << entrypoint.c_str();
        // For now we don't exclude filenames during full scans, though this might change in the future.
        // The reason being that we still want to know where the db file is (current use case), just not watch
        // it. May want to split the list in two later (scan vs watch).
        m_dispatcher.enqueue({.type = ScanType::Full, .path = entrypoint, .excludedPaths = m_excludedPaths});
      }

      for (const auto &entrypoint : m_watcherPaths) {
        qInfo() << "Enqueuing watcher scan for" << entrypoint.c_str();
        startSingleScan(entrypoint, ScanType::Watcher, m_excludedFilenames);
      }
    });
  }).detach();
}

void FileIndexer::startSingleScan(std::filesystem::path entrypoint, ScanType type,
                                  std::vector<std::string> excludedFilenames) {
  for (auto const &[id, scan] : m_dispatcher.scans()) {
    if (scan.type == type && scan.path == entrypoint) { m_dispatcher.interrupt(id); }
  }

  m_dispatcher.enqueue({.type = type,
                        .path = entrypoint,
                        .excludedFilenames = m_excludedFilenames,
                        .excludedPaths = m_excludedPaths});
}

void FileIndexer::rebuildIndex() { startFullScan(); }

void FileIndexer::markScanAsInterrupted(std::optional<FileIndexerDatabase::ScanRecord> scan) {
  if (!scan.has_value()) return;

  qWarning() << "Creating new scan after previous scan for" << scan.value().path.c_str()
             << "was unsuccessful";
  m_writer->setScanError(scan.value().id, "Interrupted");
}

void FileIndexer::start() {
  for (const auto &entrypoint : m_entrypoints) {
    auto lastScan = m_db.getLastScan(entrypoint, ScanType::Full);

    // we have never had a full scan or it failed
    if (!lastScan) {
      qInfo() << "This is our first startup for entrypoint" << entrypoint.c_str() << ", starting full scan";
      startSingleScan(entrypoint, ScanType::Full);
      continue;
    }

    // Scans marked as started when we call start() (that is, at the beginning of the program)
    // are considered failed because they were not able to finish.
    if (lastScan->status != ScanStatus::Succeeded) {
      qInfo() << "Last full scan for entrypoint" << entrypoint.c_str()
              << "did not complete successfully, marking as interrupted and starting a new full scan";
      markScanAsInterrupted(lastScan);
      startSingleScan(entrypoint, ScanType::Full);
      continue;
    }

    lastScan = m_db.getLastScan(entrypoint, ScanType::Incremental);
    if (lastScan && lastScan.value().status != ScanStatus::Succeeded) { markScanAsInterrupted(lastScan); }

    qInfo() << "Starting incremental scan for entrypoint" << entrypoint.c_str();
    startSingleScan(entrypoint, ScanType::Incremental);
  }

  for (const auto &entrypoint : m_watcherPaths) {
    startSingleScan(entrypoint, ScanType::Watcher, m_excludedFilenames);
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

  m_excludedPaths = ranges_to<std::vector>(
      preferences.value("excludedPaths").toString().split(';', Qt::SkipEmptyParts) |
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
