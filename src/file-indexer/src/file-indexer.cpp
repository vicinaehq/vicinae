#include "file-indexer/file-indexer.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/log.hpp"
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

fs::path normalizePath(const fs::path &path) { return fs::absolute(path).lexically_normal(); }

std::vector<fs::path> normalizePaths(std::vector<fs::path> paths) {
  for (auto &path : paths) {
    path = normalizePath(path);
  }

  std::ranges::sort(paths);
  auto [first, last] = std::ranges::unique(paths);
  paths.erase(first, last);

  return paths;
}

bool isSameOrDescendantOf(const fs::path &path, const fs::path &ancestor) {
  auto pathIt = path.begin();
  auto ancestorIt = ancestor.begin();

  for (; ancestorIt != ancestor.end(); ++ancestorIt, ++pathIt) {
    if (pathIt == path.end() || *pathIt != *ancestorIt) return false;
  }

  return true;
}

bool isCoveredByAny(const fs::path &path, const std::vector<fs::path> &roots) {
  return std::ranges::any_of(roots, [&](const fs::path &root) { return isSameOrDescendantOf(path, root); });
}

std::vector<fs::path> compactSubtrees(std::vector<fs::path> paths) {
  std::ranges::sort(paths, [](const fs::path &lhs, const fs::path &rhs) {
    auto const lhsSize = std::ranges::distance(lhs);
    auto const rhsSize = std::ranges::distance(rhs);
    if (lhsSize != rhsSize) return lhsSize < rhsSize;
    return lhs < rhs;
  });

  std::vector<fs::path> compacted;
  compacted.reserve(paths.size());

  for (const auto &path : paths) {
    if (!isCoveredByAny(path, compacted)) { compacted.emplace_back(path); }
  }

  return compacted;
}

bool oldRootStillCovers(const fs::path &oldRoot, const std::vector<fs::path> &newRoots) {
  return isCoveredByAny(oldRoot, newRoots);
}

bool coveredByRemainingOldRoot(const fs::path &newRoot, const std::vector<fs::path> &oldRoots,
                               const std::vector<fs::path> &newRoots) {
  return std::ranges::any_of(oldRoots, [&](const fs::path &oldRoot) {
    return isSameOrDescendantOf(newRoot, oldRoot) && oldRootStillCovers(oldRoot, newRoots);
  });
}

ReconcilePlan buildReconcilePlan(const std::vector<fs::path> &oldRoots,
                                 const std::vector<fs::path> &oldExclusions,
                                 const std::vector<fs::path> &newRoots,
                                 const std::vector<fs::path> &newExclusions) {
  ReconcilePlan plan;

  for (const auto &oldRoot : oldRoots) {
    if (!isCoveredByAny(oldRoot, newRoots)) { plan.deleteSubtrees.emplace_back(oldRoot); }
  }

  for (const auto &newRoot : newRoots) {
    if (!coveredByRemainingOldRoot(newRoot, oldRoots, newRoots)) { plan.scanRoots.emplace_back(newRoot); }
  }

  for (const auto &newExclusion : newExclusions) {
    if (!isCoveredByAny(newExclusion, oldExclusions)) { plan.deleteSubtrees.emplace_back(newExclusion); }
  }

  for (const auto &oldExclusion : oldExclusions) {
    if (isCoveredByAny(oldExclusion, newExclusions)) continue;
    if (isCoveredByAny(oldExclusion, newRoots)) { plan.scanRoots.emplace_back(oldExclusion); }
  }

  std::erase_if(plan.scanRoots, [&](const fs::path &path) { return isCoveredByAny(path, newExclusions); });
  plan.deleteSubtrees = compactSubtrees(std::move(plan.deleteSubtrees));
  plan.scanRoots = compactSubtrees(std::move(plan.scanRoots));

  return plan;
}

} // namespace

void FileIndexer::startFullScan() {
  stopHomeWatcher();
  m_dispatcher.clearPending();
  m_dispatcher.interruptAll();
  m_dispatcher.waitUntilIdle();
  markFullScanRootsPending(m_entrypoints);

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

    flog::debug() << "Starting incremental scan for entrypoint" << entrypoint.c_str();
    startSingleScan(entrypoint, ScanType::Incremental);
  }

  // otherwise the event callback starts the watcher once the full scan succeeds
  if (!needsFullScan) { startHomeWatcher(); }
}

void FileIndexer::startHomeWatcher() {
  std::scoped_lock const l(m_homeWatcherMtx);
  if (!m_homeWatcher) { m_homeWatcher = std::make_unique<HomeDirectoryWatcher>(m_dispatcher); }
}

void FileIndexer::stopHomeWatcher() {
  std::scoped_lock const l(m_homeWatcherMtx);
  m_homeWatcher.reset();
}

void FileIndexer::markFullScanRootsPending(const std::vector<fs::path> &roots) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  m_pendingFullScanRoots.reserve(m_pendingFullScanRoots.size() + roots.size());
  for (const auto &root : roots) {
    m_pendingFullScanRoots.emplace_back(normalizePath(root));
  }

  m_pendingFullScanRoots = compactSubtrees(std::move(m_pendingFullScanRoots));
}

void FileIndexer::markFullScanSucceeded(const fs::path &root) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);
  auto const normalizedRoot = normalizePath(root);

  std::erase_if(m_pendingFullScanRoots, [&](const fs::path &pendingRoot) {
    return isSameOrDescendantOf(pendingRoot, normalizedRoot);
  });
}

void FileIndexer::prunePendingFullScans(const std::vector<fs::path> &roots,
                                        const std::vector<fs::path> &exclusions) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  std::erase_if(m_pendingFullScanRoots, [&](const fs::path &pendingRoot) {
    return !isCoveredByAny(pendingRoot, roots) || isCoveredByAny(pendingRoot, exclusions);
  });
}

std::vector<fs::path> FileIndexer::pendingFullScanRootsFor(const std::vector<fs::path> &roots,
                                                           const std::vector<fs::path> &exclusions) {
  std::scoped_lock const l(m_pendingFullScanRootsMtx);

  std::vector<fs::path> pendingRoots;
  pendingRoots.reserve(m_pendingFullScanRoots.size());

  for (const auto &pendingRoot : m_pendingFullScanRoots) {
    if (isCoveredByAny(pendingRoot, roots) && !isCoveredByAny(pendingRoot, exclusions)) {
      pendingRoots.emplace_back(pendingRoot);
    }
  }

  return compactSubtrees(std::move(pendingRoots));
}

void FileIndexer::setConfig(std::vector<fs::path> paths, std::vector<fs::path> excludedPaths) {
  m_entrypoints = normalizePaths(std::move(paths));
  m_excludedPaths = normalizePaths(std::move(excludedPaths));

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};
}

void FileIndexer::applyConfig(std::vector<fs::path> paths, std::vector<fs::path> excludedPaths) {
  auto newEntryPoints = normalizePaths(std::move(paths));
  auto newExcludedPaths = normalizePaths(std::move(excludedPaths));

  if (newEntryPoints == m_entrypoints && newExcludedPaths == m_excludedPaths) return;

  auto plan = buildReconcilePlan(m_entrypoints, m_excludedPaths, newEntryPoints, newExcludedPaths);
  prunePendingFullScans(newEntryPoints, newExcludedPaths);
  auto pendingRoots = pendingFullScanRootsFor(newEntryPoints, newExcludedPaths);
  plan.scanRoots.reserve(plan.scanRoots.size() + pendingRoots.size());
  for (auto &pendingRoot : pendingRoots) {
    plan.scanRoots.emplace_back(std::move(pendingRoot));
  }
  plan.scanRoots = compactSubtrees(std::move(plan.scanRoots));

  m_entrypoints = std::move(newEntryPoints);
  m_excludedPaths = std::move(newExcludedPaths);

  std::string const databaseFilename = FileIndexerDatabase::getDatabasePath().filename().string();
  m_excludedFilenames = {databaseFilename, databaseFilename + "-wal"};

  flog::info() << "Applying file indexer configuration change: deleting " << plan.deleteSubtrees.size()
               << " subtrees and scanning " << plan.scanRoots.size() << " roots";

  stopHomeWatcher();
  m_dispatcher.clearPending();
  m_dispatcher.interruptAll();
  m_dispatcher.waitUntilIdle();

  if (!plan.deleteSubtrees.empty()) {
    auto onComplete = plan.scanRoots.empty() ? [this]() { startHomeWatcher(); } : std::function<void()>{};
    m_writer->deleteIndexedFiles(std::move(plan.deleteSubtrees), std::move(onComplete));
  }

  for (const auto &root : plan.scanRoots) {
    markFullScanRootsPending({root});
    m_dispatcher.enqueue({.path = root, .data = FullScan{.excludedPaths = m_excludedPaths}, .notify = true});
  }

  if (plan.deleteSubtrees.empty() && plan.scanRoots.empty()) { startHomeWatcher(); }
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
    if (event.type == ScanType::Full && event.status == ScanStatus::Succeeded) {
      markFullScanSucceeded(event.entrypoint);
    }

    // keep the typo-correction vocabulary loosely in sync with the index
    if (event.status == ScanStatus::Succeeded && shouldRebuildVocabulary()) {
      m_writer->rebuildSpellfixVocabulary();
    }
    if (event.status == ScanStatus::Succeeded && event.type == ScanType::Full) { startHomeWatcher(); }
    if (m_scanEventCallback) { m_scanEventCallback(event); }
  });
}
