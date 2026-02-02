#include "utils/utils.hpp"
#include "incremental-scanner.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include <QDebug>
#include <memory>
#include <unordered_set>

namespace fs = std::filesystem;

void IncrementalScanner::processDirectory(const std::filesystem::path &root) {
  auto indexedFiles = m_read_db->listIndexedDirectoryFiles(root);
  std::unordered_set<fs::path> existingFiles(indexedFiles.begin(), indexedFiles.end());
  std::unordered_set<fs::path> currentFiles;
  std::vector<fs::path> deletedFiles;
  std::error_code ec;

  currentFiles.insert(root);

  for (const auto &entry : fs::directory_iterator(root, ec)) {
    if (ec) continue;

    // XXX - We may want to differenciate between new files and already existing later
    // especially if we start indexing file content as well.
    currentFiles.insert(entry.path());
  }

  for (const auto &path : indexedFiles) {
    if (currentFiles.find(path) == currentFiles.end()) { deletedFiles.emplace_back(path); }
  }

  m_writer->deleteIndexedFiles(deletedFiles);
  m_writer->indexFiles(std::move(ranges_to<std::vector>(currentFiles)));
}

std::vector<fs::path>
IncrementalScanner::getScannableDirectories(const fs::path &path, std::optional<size_t> maxDepth,
                                            const std::vector<fs::path> &excludedPaths) const {
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
    qCritical() << "No previous successful scan found for incremental scan at" << path.c_str();
    return scannableDirs;
  }

  walker.setMaxDepth(maxDepth);
  walker.setExcludedPaths(excludedPaths);
  walker.walk(path, [&](const fs::directory_entry &entry) {
    if (!entry.is_directory(ec)) return;
    bool shouldProcess = shouldProcessEntry(entry, lastSuccessfulScan->createdAt);
    if (shouldProcess) { scannableDirs.emplace_back(entry.path()); }
  });

  return scannableDirs;
}

bool IncrementalScanner::shouldProcessEntry(const fs::directory_entry &entry,
                                            const QDateTime &cutOffDateTime) const {
  std::error_code ec;
  bool shouldProcess = false;

  if (auto lastModified = fs::last_write_time(entry, ec); !ec) {
    using namespace std::chrono;
    auto sctp = clock_cast<system_clock>(lastModified);
    auto lastModifiedDate =
        QDateTime::fromSecsSinceEpoch(duration_cast<seconds>(sctp.time_since_epoch()).count());

    // Only process if directory was modified after the cutoff or equal (to be safe)
    return lastModifiedDate >= cutOffDateTime;
  }

  // If we can't get mtime, process anyway to be safe
  return true;
}

void IncrementalScanner::scan(const Scan &scan) {
  for (const auto &dir : getScannableDirectories(scan.path, scan.maxDepth, scan.excludedPaths)) {
    processDirectory(dir);
  }
}

IncrementalScanner::IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &sc,
                                       FinishCallback callback)
    : AbstractScanner(writer, sc, callback) {
  m_scanThread = std::thread([this, sc]() {
    m_read_db = std::make_unique<FileIndexerDatabase>();
    start(sc);

    try {
      scan(sc);
      finish();
    } catch (const std::exception &error) {
      qCritical() << "Caught exception during incremental scan" << error.what();
      fail();
    }
  });
}

void IncrementalScanner::interrupt() {
  setInterruptFlag();
  // TODO: Actually add signalling
}

void IncrementalScanner::join() { m_scanThread.join(); }
