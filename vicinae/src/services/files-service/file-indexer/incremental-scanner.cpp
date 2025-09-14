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
  m_writer->indexFiles(ranges_to<std::vector>(currentFiles));
}

std::vector<fs::path> IncrementalScanner::getScannableDirectories(const fs::path &path,
                                                                  std::optional<size_t> maxDepth) const {
  std::vector<fs::path> scannableDirs;
  std::error_code ec;
  FileSystemWalker walker;

  scannableDirs.emplace_back(path);

  walker.setMaxDepth(maxDepth);
  walker.walk(path, [&](const fs::directory_entry &entry) {
    if (!entry.is_directory(ec)) return;

    auto lastScanned = m_read_db->retrieveIndexedLastModified(entry);

    if (auto lastModified = fs::last_write_time(entry, ec); lastScanned.has_value() && !ec) {
      using namespace std::chrono;
      auto sctp = clock_cast<system_clock>(lastModified);
      auto lastModifiedDate =
          QDateTime::fromSecsSinceEpoch(duration_cast<seconds>(sctp.time_since_epoch()).count());

      if (lastScanned < lastModifiedDate) { scannableDirs.emplace_back(entry.path()); }
    }
  });

  return scannableDirs;
}

void IncrementalScanner::scan(const Scan &scan) {
  for (const auto &dir : getScannableDirectories(scan.path, scan.maxDepth)) {
    processDirectory(dir);
  }
}

IncrementalScanner::IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &sc, FinishCallback callback)
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
