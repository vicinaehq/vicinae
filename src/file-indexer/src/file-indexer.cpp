#include "file-indexer/file-indexer.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/log.hpp"
#include <filesystem>
#include <thread>

namespace fs = std::filesystem;

void FileIndexer::startFullScan() {
  m_dispatcher.interruptAll();

  // Prevent starting the full scan and watchers before deletion is done
  std::thread([this]() {
    flog::info() << "Starting full scan, clearing existing index...";

    // Delete all indexed files, then enqueue new scans in the completion callback
    m_writer->deleteAllIndexedFiles([this]() {
      flog::info() << "Existing index cleared, compacting database...";

      m_writer->compact([this]() {
        flog::info() << "Database compacted, enqueuing full scan tasks...";

        for (const auto &entrypoint : m_entrypoints) {
          flog::info() << "Enqueuing full scan for" << entrypoint.c_str();
          // For now we don't exclude filenames during full scans, though this might change in the future.
          // The reason being that we still want to know where the db file is (current use case), just not
          // watch it. May want to split the list in two later (scan vs watch).
          m_dispatcher.enqueue(
              {.type = ScanType::Full, .path = entrypoint, .excludedPaths = m_excludedPaths});
        }

        for (const auto &entrypoint : m_watcherPaths) {
          flog::info() << "Enqueuing watcher scan for" << entrypoint.c_str();
          startSingleScan(entrypoint, ScanType::Watcher, m_excludedFilenames);
        }
      });
    });
  }).detach();
}

void FileIndexer::startSingleScan(const std::filesystem::path &entrypoint, ScanType type,
                                  const std::vector<std::string> &excludedFilenames) {
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

  flog::warn() << "Creating new scan after previous scan for" << scan.value().path.c_str()
               << "was unsuccessful";
  m_writer->setScanError(scan.value().id, "Interrupted");
}

void FileIndexer::start() {
  for (const auto &entrypoint : m_entrypoints) {
    auto lastScan = m_db.getLastScan(entrypoint, ScanType::Full);

    // we have never had a full scan or it failed
    if (!lastScan) {
      flog::info() << "This is our first startup for entrypoint" << entrypoint.c_str()
                   << ", starting full scan";
      startSingleScan(entrypoint, ScanType::Full);
      continue;
    }

    // Scans marked as started when we call start() (that is, at the beginning of the program)
    // are considered failed because they were not able to finish.
    if (lastScan->status != ScanStatus::Succeeded) {
      flog::info() << "Last full scan for entrypoint" << entrypoint.c_str()
                   << "did not complete successfully, marking as interrupted and starting a new full scan";
      markScanAsInterrupted(lastScan);
      startSingleScan(entrypoint, ScanType::Full);
      continue;
    }

    lastScan = m_db.getLastScan(entrypoint, ScanType::Incremental);
    if (lastScan && lastScan.value().status != ScanStatus::Succeeded) { markScanAsInterrupted(lastScan); }

    flog::info() << "Starting incremental scan for entrypoint" << entrypoint.c_str();
    startSingleScan(entrypoint, ScanType::Incremental);
  }

  for (const auto &entrypoint : m_watcherPaths) {
    startSingleScan(entrypoint, ScanType::Watcher, m_excludedFilenames);
  }
}

void FileIndexer::setConfig(std::vector<std::filesystem::path> paths,
                            std::vector<std::filesystem::path> excludedPaths,
                            std::vector<std::filesystem::path> watcherPaths) {
  m_entrypoints = std::move(paths);
  m_excludedPaths = std::move(excludedPaths);
  m_watcherPaths = std::move(watcherPaths);

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};
}

std::vector<IndexerFileResult> FileIndexer::query(std::string_view view, const Pagination &pagination) {
  return m_queryEngine.query(view, pagination);
}

FileIndexer::FileIndexer() : m_writer(std::make_shared<DbWriter>()), m_dispatcher(m_writer) {
  m_db.runMigrations();
}
