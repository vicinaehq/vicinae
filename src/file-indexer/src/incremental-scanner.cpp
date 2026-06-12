#include "file-indexer/incremental-scanner.hpp"
#include "file-indexer/filesystem-walker.hpp"
#include "file-indexer/log.hpp"
#include <chrono>
#include <deque>
#include <memory>
#include <ranges>
#include <utility>

namespace fs = std::filesystem;

void IncrementalScanner::processDirectory(const fs::path &root, const EntryCallback &onEntry) {
  auto indexedFiles = m_read_db->listIndexedDirectoryFiles(root);
  std::vector<fs::path> deletedFiles;
  std::error_code ec;

  m_currentEntries.clear();
  m_currentEntries.insert(root);

  for (const auto &entry : fs::directory_iterator(root, ec)) {
    if (ec) continue;
    if (!m_filter.shouldVisit(entry)) continue;

    m_currentEntries.insert(entry.path());

    if (onEntry) { onEntry(entry, !indexedFiles.contains(entry.path())); }
  }

  for (const auto &path : indexedFiles) {
    if (!m_currentEntries.contains(path)) { deletedFiles.emplace_back(path); }
  }

  m_writer->deleteIndexedFiles(std::move(deletedFiles));
  m_writer->indexFiles(m_currentEntries | std::ranges::to<std::vector>());
  reportProgress(m_currentEntries.size());
}

std::vector<fs::path>
IncrementalScanner::getScannableDirectories(const fs::path &path, std::optional<size_t> maxDepth,
                                            const std::vector<fs::path> &excludedPaths) {
  std::vector<fs::path> scannableDirs;
  std::error_code ec;
  FileSystemWalker walker;

  /**
   * We could have used the max last modified time of indexed files under this path
   * but in the event that a file was missed for whatever reason, it would never be picked up again.
   * Since it is assumed that incremental and full scans are more reliable than watchers, we simply use the
   * last successful scan time (full or incremental).
   */

  scannableDirs.emplace_back(path);
  auto lastSuccessfulScan = m_read_db->getLastSuccessfulScan(path);

  // This should not happen as we would have run a full scan before coming here
  if (!lastSuccessfulScan.has_value()) {
    flog::error() << "No previous successful scan found for incremental scan at" << path.c_str();
    return scannableDirs;
  }

  walker.setMaxDepth(maxDepth);
  walker.setExcludedPaths(excludedPaths);
  walker.walk(path, [&](const fs::directory_entry &entry) {
    reportProgress();
    if (!entry.is_directory(ec)) return;
    bool const shouldProcess = shouldProcessEntry(entry, lastSuccessfulScan->createdAt);
    if (shouldProcess) { scannableDirs.emplace_back(entry.path()); }
  });

  return scannableDirs;
}

bool IncrementalScanner::shouldProcessEntry(const fs::directory_entry &entry, int64_t cutOffSeconds) const {
  std::error_code ec;

  if (auto lastModified = fs::last_write_time(entry, ec); !ec) {
    using namespace std::chrono;
    auto sctp = clock_cast<system_clock>(lastModified);
    auto const lastModifiedSeconds =
        static_cast<int64_t>(duration_cast<seconds>(sctp.time_since_epoch()).count());

    // Only process if directory was modified after the cutoff or equal (to be safe)
    return lastModifiedSeconds >= cutOffSeconds;
  }

  // If we can't get mtime, process anyway to be safe
  return true;
}

void IncrementalScanner::exhaustiveScan(const Scan &scan) {
  for (const auto &dir : getScannableDirectories(scan.path, scan.maxDepth, scan.excludedPaths)) {
    if (isInterrupted()) break;
    processDirectory(dir);
  }
}

void IncrementalScanner::prunedScan(const Scan &scan) {
  // most scanned paths never have their own scan record, but a successful scan
  // of an ancestor (typically the entrypoint's full scan) covered this subtree:
  // its timestamp is a valid cutoff. Without it the cutoff degrades to 0 and
  // first contact descends the entire subtree.
  int64_t cutOffSeconds = 0;

  for (fs::path path = scan.path;; path = path.parent_path()) {
    if (auto lastSuccessfulScan = m_read_db->getLastSuccessfulScan(path)) {
      cutOffSeconds = lastSuccessfulScan->createdAt;
      break;
    }
    if (path == path.parent_path()) break;
  }

  std::deque<fs::path> queue;
  std::error_code ec;

  auto collectChangedDirs = [&](const fs::directory_entry &entry, bool isNew) {
    if (!entry.is_directory(ec)) return;
    if (isNew || shouldProcessEntry(entry, cutOffSeconds)) { queue.emplace_back(entry.path()); }
  };

  queue.emplace_back(scan.path);

  while (!queue.empty() && !isInterrupted()) {
    auto dir = std::move(queue.front());
    queue.pop_front();
    processDirectory(dir, collectChangedDirs);
  }
}

void IncrementalScanner::scan(const Scan &scan) {
  m_filter.setExcludedPaths(scan.excludedPaths);
  m_filter.setExcludedFilenames(scan.excludedFilenames);

  switch (scan.mode) {
  case ScanMode::Exhaustive:
    exhaustiveScan(scan);
    break;
  case ScanMode::Pruned:
    prunedScan(scan);
    break;
  }
}

IncrementalScanner::IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &sc,
                                       StatusCallback callback)
    : AbstractScanner(std::move(writer), sc, std::move(callback)) {
  m_scanThread = std::thread([this, sc]() {
    m_read_db = std::make_unique<FileIndexerDatabase>();
    start(sc);

    try {
      scan(sc);
      finish();
    } catch (const std::exception &error) {
      flog::error() << "Caught exception during incremental scan" << error.what();
      fail();
    }
  });
}

void IncrementalScanner::interrupt() { setInterruptFlag(); }

void IncrementalScanner::join() { m_scanThread.join(); }
