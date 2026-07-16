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
  m_pacer.checkpoint();

  auto indexedFiles = m_readDb.listIndexedDirectoryFiles(root);
  std::vector<fs::path> deletedFiles;
  std::vector<FileEvent> events;
  std::error_code ec;

  m_currentEntries.clear();
  m_currentEntries.insert(root);
  events.emplace_back(FileEventType::Modify, root, fs::last_write_time(root, ec), true, std::nullopt);

  for (const auto &entry : fs::directory_iterator(root, ec)) {
    if (ec) continue;
    if (!m_filter.shouldVisit(entry)) continue;

    bool const isDirectory = entry.is_directory(ec);
    m_currentEntries.insert(entry.path());
    events.emplace_back(FileEventType::Modify, entry.path(), entry.last_write_time(ec), isDirectory,
                        file_indexer::fileSizeBytesFor(entry.path(), isDirectory));

    if (onEntry) { onEntry(entry, !indexedFiles.contains(entry.path())); }
  }

  for (const auto &path : indexedFiles) {
    if (!m_currentEntries.contains(path)) { deletedFiles.emplace_back(path); }
  }

  auto const processedCount = events.size();

  m_writer->deleteIndexedFiles(std::move(deletedFiles));
  m_writer->indexEvents(std::move(events));
  reportProgress(processedCount);
}

std::vector<fs::path>
IncrementalScanner::getScannableDirectories(const fs::path &path, std::optional<size_t> maxDepth,
                                            const std::vector<fs::path> &excludedPaths) {
  std::vector<fs::path> scannableDirs;
  std::error_code ec;
  FileSystemWalker walker;

  scannableDirs.emplace_back(path);
  auto lastSuccessfulScan = m_readDb.getLastSuccessfulScan(path);

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

    return lastModifiedSeconds >= cutOffSeconds || !m_readDb.tracksFile(entry.path());
  }

  return true;
}

void IncrementalScanner::exhaustiveScan(const fs::path &path, const IncrementalScan &scan) {
  std::deque<fs::path> newDirs;
  std::unordered_set<fs::path> processed;
  std::error_code ec;

  auto collectNewDirs = [&](const fs::directory_entry &entry, bool isNew) {
    if (isNew && entry.is_directory(ec)) { newDirs.emplace_back(entry.path()); }
  };

  for (const auto &dir : getScannableDirectories(path, scan.maxDepth, scan.excludedPaths)) {
    if (isInterrupted()) break;
    processed.insert(dir);
    processDirectory(dir, collectNewDirs);
  }

  while (!newDirs.empty() && !isInterrupted()) {
    auto dir = std::move(newDirs.front());
    newDirs.pop_front();

    if (!processed.insert(dir).second) continue;
    processDirectory(dir, collectNewDirs);
  }
}

void IncrementalScanner::prunedScan(const fs::path &scanPath, const IncrementalScan &scan) {
  int64_t cutOffSeconds = 0;

  for (fs::path path = scanPath;; path = path.parent_path()) {
    if (auto lastSuccessfulScan = m_readDb.getLastSuccessfulScan(path)) {
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

  queue.emplace_back(scanPath);

  while (!queue.empty() && !isInterrupted()) {
    auto dir = std::move(queue.front());
    queue.pop_front();
    processDirectory(dir, collectChangedDirs);
  }
}

void IncrementalScanner::scan(const fs::path &path, const IncrementalScan &scan) {
  m_filter.setExcludedPaths(scan.excludedPaths);
  m_filter.setExcludedFilenames(scan.excludedFilenames);

  switch (scan.mode) {
  case ScanMode::Exhaustive:
    exhaustiveScan(path, scan);
    break;
  case ScanMode::Pruned:
    prunedScan(path, scan);
    break;
  }
}

IncrementalScanner::IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &sc,
                                       FileIndexerDatabase &readDb, StatusCallback callback)
    : AbstractScanner(std::move(writer), sc, std::move(callback)), m_readDb(readDb) {}

void IncrementalScanner::run() {
  start();

  try {
    scan(m_scan.path, std::get<IncrementalScan>(m_scan.data));
    finish();
  } catch (const std::exception &error) {
    flog::error() << "Caught exception during incremental scan" << error.what();
    fail();
  }
}

void IncrementalScanner::interrupt() { setInterruptFlag(); }
