#include "incremental-scanner.hpp"
#include "abstract-scanner.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include "utils.hpp"
#include <memory>
#include <qlogging.h>
#include <unordered_set>

namespace fs = std::filesystem;

void IncrementalScanner::processDirectory(const std::filesystem::path &root) {
  auto indexedFiles = m_db->listIndexedDirectoryFiles(root);
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

  m_db->deleteIndexedFiles(deletedFiles);
  m_db->indexFiles(ranges_to<std::vector>(currentFiles));
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

    auto lastScanned = m_db->retrieveIndexedLastModified(entry);

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

void IncrementalScanner::run() {
  AbstractScanner::run();

  m_db = std::make_unique<FileIndexerDatabase>();

  while (true) {
    auto expected = awaitScan();
    if (!expected.has_value()) break;

    const Scan &sc = *expected;

    auto result = m_db->createScan(sc.path, sc.type);

    if (!result) {
      qWarning() << "Not scanning" << sc.path << "because scan record creation failed with error"
                 << result.error();
      continue;
    }

    auto scanRecord = result.value();

    m_db->updateScanStatus(scanRecord.id, FileIndexerDatabase::ScanStatus::Started);

    try {
      scan(sc);
      m_db->updateScanStatus(scanRecord.id, FileIndexerDatabase::ScanStatus::Finished);
    } catch (const std::exception &error) {
      qCritical() << "Caught exception during incremental scan" << error.what();
      m_db->updateScanStatus(scanRecord.id, FileIndexerDatabase::ScanStatus::Failed);
    }

    finishScan(sc);
  }
}

void IncrementalScanner::stop(bool regurgitate) { AbstractScanner::stop(regurgitate); }
