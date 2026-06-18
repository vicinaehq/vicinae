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

    // no VACUUM here: the freed pages are about to be reused by the rescan
    m_writer->deleteAllIndexedFiles([this]() {
      flog::info() << "Existing index cleared, enqueuing full scan tasks...";

      for (const auto &entrypoint : m_entrypoints) {
        flog::info() << "Enqueuing full scan for" << entrypoint.c_str();
        // For now we don't exclude filenames during full scans, though this might change in the future.
        // The reason being that we still want to know where the db file is (current use case), just not
        // watch it. May want to split the list in two later (scan vs watch).
        m_dispatcher.enqueue(
            {.path = entrypoint, .data = FullScan{.excludedPaths = m_excludedPaths}, .notify = true});
      }
    });
  }).detach();
}

void FileIndexer::startSingleScan(const fs::path &entrypoint, ScanType type,
                                  const std::vector<std::string> &excludedFilenames) {
  for (auto const &[id, scan] : m_dispatcher.scans()) {
    if (scan.type() == type && scan.path == entrypoint) { m_dispatcher.interrupt(id); }
  }

  if (type == ScanType::Full) {
    m_dispatcher.enqueue(
        {.path = entrypoint, .data = FullScan{.excludedPaths = m_excludedPaths}, .notify = true});
    return;
  }

  m_dispatcher.enqueue(
      {.path = entrypoint,
       .data = IncrementalScan{.excludedFilenames = m_excludedFilenames, .excludedPaths = m_excludedPaths},
       .notify = true});
}

void FileIndexer::rebuildIndex() { startFullScan(); }

void FileIndexer::markScanAsInterrupted(std::optional<FileIndexerDatabase::ScanRecord> scan) {
  if (!scan.has_value()) return;

  flog::warn() << "Creating new scan after previous scan for" << scan.value().path.c_str()
               << "was unsuccessful";
  m_writer->setScanError(scan.value().id, "Interrupted");
}

void FileIndexer::start() {
  if (!m_db.isOpen()) {
    flog::error() << "File indexer database is not open, refusing to start scans";
    return;
  }

  // make typo corrections available before the first scan of this session completes
  if (!m_db.hasSpellfixVocabulary()) { m_writer->rebuildSpellfixVocabulary(); }

  bool needsFullScan = false;

  for (const auto &entrypoint : m_entrypoints) {
    auto lastScan = m_db.getLastScan(entrypoint, ScanType::Full);

    // we have never had a full scan or it failed
    if (!lastScan) {
      flog::info() << "This is our first startup for entrypoint" << entrypoint.c_str()
                   << ", starting full scan";
      startSingleScan(entrypoint, ScanType::Full);
      needsFullScan = true;
      continue;
    }

    // Scans marked as started when we call start() (that is, at the beginning of the program)
    // are considered failed because they were not able to finish.
    if (lastScan->status != ScanStatus::Succeeded) {
      flog::info() << "Last full scan for entrypoint" << entrypoint.c_str()
                   << "did not complete successfully, marking as interrupted and starting a new full scan";
      markScanAsInterrupted(lastScan);
      startSingleScan(entrypoint, ScanType::Full);
      needsFullScan = true;
      continue;
    }

    lastScan = m_db.getLastScan(entrypoint, ScanType::Incremental);
    if (lastScan && lastScan.value().status != ScanStatus::Succeeded) { markScanAsInterrupted(lastScan); }

    flog::info() << "Starting incremental scan for entrypoint" << entrypoint.c_str();
    startSingleScan(entrypoint, ScanType::Incremental);
  }

  // otherwise the event callback starts the watcher once the full scan succeeds
  if (!needsFullScan) { startHomeWatcher(); }
}

void FileIndexer::startHomeWatcher() {
  std::scoped_lock const l(m_homeWatcherMtx);
  if (!m_homeWatcher) { m_homeWatcher = std::make_unique<HomeDirectoryWatcher>(m_dispatcher); }
}

void FileIndexer::setConfig(std::vector<fs::path> paths, std::vector<fs::path> excludedPaths) {
  m_entrypoints = std::move(paths);
  m_excludedPaths = std::move(excludedPaths);

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};
}

std::vector<IndexerFileResult> FileIndexer::query(std::string_view view, int limit,
                                                  const FileIndexerQueryEngine::QueryOptions &options) {
  if (!m_db.isOpen() || !m_queryEngine.isAvailable()) return {};
  return m_queryEngine.query(view, limit, options);
}

bool FileIndexer::shouldRebuildVocabulary() {
  std::scoped_lock const l(m_vocabRebuildMtx);
  auto const now = std::chrono::steady_clock::now();

  if (now - m_lastVocabRebuild < VOCAB_REBUILD_MIN_INTERVAL) return false;

  m_lastVocabRebuild = now;
  return true;
}

FileIndexer::FileIndexer() : m_writer(std::make_shared<DbWriter>()), m_dispatcher(m_writer) {
  if (m_db.isOpen()) { m_db.init(); }

  m_dispatcher.setEventCallback([this](const ScanEvent &event) {
    // keep the typo-correction vocabulary loosely in sync with the index
    if (event.status == ScanStatus::Succeeded && shouldRebuildVocabulary()) {
      m_writer->rebuildSpellfixVocabulary();
    }
    if (event.status == ScanStatus::Succeeded && event.type == ScanType::Full) { startHomeWatcher(); }
    if (m_scanEventCallback) { m_scanEventCallback(event); }
  });
}
