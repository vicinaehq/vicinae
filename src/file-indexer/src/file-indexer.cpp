#include "file-indexer/file-indexer.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/util.hpp"
#include <algorithm>
#include <filesystem>
#include <functional>
#include <ranges>
#include <thread>

namespace fs = std::filesystem;

namespace {

struct ReconcilePlan {
  std::vector<fs::path> deleteSubtrees;
  std::vector<fs::path> scanRoots;
};

bool oldRootStillCovers(const fs::path &oldRoot, const std::vector<fs::path> &newRoots) {
  return file_indexer::isCoveredByAny(oldRoot, newRoots);
}

bool coveredByRemainingOldRoot(const fs::path &newRoot, const std::vector<fs::path> &oldRoots,
                               const std::vector<fs::path> &newRoots) {
  return std::ranges::any_of(oldRoots, [&](const fs::path &oldRoot) {
    return file_indexer::isSameOrDescendantOf(newRoot, oldRoot) && oldRootStillCovers(oldRoot, newRoots);
  });
}

ReconcilePlan buildReconcilePlan(const std::vector<fs::path> &oldRoots,
                                 const std::vector<fs::path> &oldExclusions,
                                 const std::vector<fs::path> &newRoots,
                                 const std::vector<fs::path> &newExclusions) {
  ReconcilePlan plan;

  for (const auto &oldRoot : oldRoots) {
    if (!file_indexer::isCoveredByAny(oldRoot, newRoots)) { plan.deleteSubtrees.emplace_back(oldRoot); }
  }

  for (const auto &newRoot : newRoots) {
    if (!coveredByRemainingOldRoot(newRoot, oldRoots, newRoots)) { plan.scanRoots.emplace_back(newRoot); }
  }

  for (const auto &newExclusion : newExclusions) {
    if (!file_indexer::isCoveredByAny(newExclusion, oldExclusions)) {
      plan.deleteSubtrees.emplace_back(newExclusion);
    }
  }

  for (const auto &oldExclusion : oldExclusions) {
    if (file_indexer::isCoveredByAny(oldExclusion, newExclusions)) continue;
    if (file_indexer::isCoveredByAny(oldExclusion, newRoots)) { plan.scanRoots.emplace_back(oldExclusion); }
  }

  std::erase_if(plan.scanRoots,
                [&](const fs::path &path) { return file_indexer::isCoveredByAny(path, newExclusions); });
  plan.deleteSubtrees = file_indexer::compactSubtrees(std::move(plan.deleteSubtrees));
  plan.scanRoots = file_indexer::compactSubtrees(std::move(plan.scanRoots));

  return plan;
}

} // namespace

void FileIndexer::startFullScan() {
  stopFileSystemWatcher();
  m_dispatcher.clearPending();
  m_dispatcher.interruptAll();
  m_dispatcher.waitUntilIdle();
  markFullScanRootsPending(m_entrypoints);

  std::thread([this]() {
    flog::info() << "Starting full scan, clearing existing index...";

    m_writer->deleteAllIndexedFiles([this]() {
      flog::info() << "Existing index cleared, enqueuing full scan tasks...";

      for (const auto &entrypoint : m_entrypoints) {
        flog::info() << "Enqueuing full scan for" << entrypoint.c_str();
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
    markFullScanRootsPending({entrypoint});
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

    if (!lastScan) {
      flog::info() << "This is our first startup for entrypoint" << entrypoint.c_str()
                   << ", starting full scan";
      startSingleScan(entrypoint, ScanType::Full);
      needsFullScan = true;
      continue;
    }

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

    flog::debug() << "Starting incremental scan for entrypoint" << entrypoint.c_str();
    startSingleScan(entrypoint, ScanType::Incremental);
  }

  if (!needsFullScan) { startFileSystemWatcher(); }
}

void FileIndexer::startFileSystemWatcher() {
  std::scoped_lock const l(m_fileSystemWatcherMtx);
  if (!m_fileSystemWatcher) {
    m_fileSystemWatcher = std::make_unique<FileSystemWatcher>(m_dispatcher, m_entrypoints, m_excludedPaths,
                                                              m_excludedFilenames);
  }
}

void FileIndexer::stopFileSystemWatcher() {
  std::scoped_lock const l(m_fileSystemWatcherMtx);
  m_fileSystemWatcher.reset();
}

void FileIndexer::markFullScanRootsPending(const std::vector<fs::path> &roots) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  m_pendingFullScanRoots.reserve(m_pendingFullScanRoots.size() + roots.size());
  for (const auto &root : roots) {
    m_pendingFullScanRoots.emplace_back(file_indexer::normalizePath(root));
  }

  m_pendingFullScanRoots = file_indexer::compactSubtrees(std::move(m_pendingFullScanRoots));
}

void FileIndexer::markFullScanSucceeded(const fs::path &root) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);
  auto const normalizedRoot = file_indexer::normalizePath(root);

  std::erase_if(m_pendingFullScanRoots, [&](const fs::path &pendingRoot) {
    return file_indexer::isSameOrDescendantOf(pendingRoot, normalizedRoot);
  });
}

bool FileIndexer::hasPendingFullScanRoots() {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);
  return !m_pendingFullScanRoots.empty();
}

void FileIndexer::prunePendingFullScans(const std::vector<fs::path> &roots,
                                        const std::vector<fs::path> &exclusions) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  std::erase_if(m_pendingFullScanRoots, [&](const fs::path &pendingRoot) {
    return !file_indexer::isCoveredByAny(pendingRoot, roots) ||
           file_indexer::isCoveredByAny(pendingRoot, exclusions);
  });
}

std::vector<fs::path> FileIndexer::pendingFullScanRootsFor(const std::vector<fs::path> &roots,
                                                           const std::vector<fs::path> &exclusions) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  std::vector<fs::path> pendingRoots;
  pendingRoots.reserve(m_pendingFullScanRoots.size());

  for (const auto &pendingRoot : m_pendingFullScanRoots) {
    if (file_indexer::isCoveredByAny(pendingRoot, roots) &&
        !file_indexer::isCoveredByAny(pendingRoot, exclusions)) {
      pendingRoots.emplace_back(pendingRoot);
    }
  }

  return file_indexer::compactSubtrees(std::move(pendingRoots));
}

void FileIndexer::setConfig(std::vector<fs::path> paths, std::vector<fs::path> excludedPaths) {
  m_entrypoints = file_indexer::normalizePaths(std::move(paths));
  m_excludedPaths = file_indexer::normalizePaths(std::move(excludedPaths));

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};
}

void FileIndexer::applyConfig(std::vector<fs::path> paths, std::vector<fs::path> excludedPaths) {
  auto newEntryPoints = file_indexer::normalizePaths(std::move(paths));
  auto newExcludedPaths = file_indexer::normalizePaths(std::move(excludedPaths));

  if (newEntryPoints == m_entrypoints && newExcludedPaths == m_excludedPaths) return;

  auto plan = buildReconcilePlan(m_entrypoints, m_excludedPaths, newEntryPoints, newExcludedPaths);
  prunePendingFullScans(newEntryPoints, newExcludedPaths);
  auto pendingRoots = pendingFullScanRootsFor(newEntryPoints, newExcludedPaths);
  plan.scanRoots.reserve(plan.scanRoots.size() + pendingRoots.size());
  for (auto &pendingRoot : pendingRoots) {
    plan.scanRoots.emplace_back(std::move(pendingRoot));
  }
  plan.scanRoots = file_indexer::compactSubtrees(std::move(plan.scanRoots));

  m_entrypoints = std::move(newEntryPoints);
  m_excludedPaths = std::move(newExcludedPaths);

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};

  flog::info() << "Applying file indexer configuration change: deleting " << plan.deleteSubtrees.size()
               << " subtrees and scanning " << plan.scanRoots.size() << " roots";

  stopFileSystemWatcher();
  m_dispatcher.clearPending();
  m_dispatcher.interruptAll();
  m_dispatcher.waitUntilIdle();

  if (!plan.deleteSubtrees.empty()) {
    auto onComplete =
        plan.scanRoots.empty() ? [this]() { startFileSystemWatcher(); } : std::function<void()>{};
    m_writer->deleteIndexedFiles(std::move(plan.deleteSubtrees), std::move(onComplete));
  }

  for (const auto &root : plan.scanRoots) {
    markFullScanRootsPending({root});
    m_dispatcher.enqueue({.path = root, .data = FullScan{.excludedPaths = m_excludedPaths}, .notify = true});
  }

  if (plan.deleteSubtrees.empty() && plan.scanRoots.empty()) { startFileSystemWatcher(); }
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
  if (m_db.isOpen()) {
    m_db.init();
    m_writer->pruneScanHistory(
        std::chrono::duration_cast<std::chrono::seconds>(SCAN_HISTORY_MAX_AGE).count());
    m_writer->compactIfNeeded();
  }

  m_dispatcher.setEventCallback([this](const ScanEvent &event) {
    if (event.type == ScanType::Full && event.status == ScanStatus::Succeeded) {
      markFullScanSucceeded(event.entrypoint);
    }

    if (event.status == ScanStatus::Succeeded && shouldRebuildVocabulary()) {
      m_writer->rebuildSpellfixVocabulary();
    }
    if (event.status == ScanStatus::Succeeded && event.type == ScanType::Full && !hasPendingFullScanRoots()) {
      startFileSystemWatcher();
    }
    if (m_scanEventCallback) { m_scanEventCallback(event); }
  });
}
