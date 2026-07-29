#include "file-indexer/indexer-scanner.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/log.hpp"
#include <utility>

namespace fs = std::filesystem;

void IndexerScanner::scan(const Scan &scan) {
  const auto &fullScan = std::get<FullScan>(scan.data);
  std::vector<FileEvent> batchedIndex;

  batchedIndex.reserve(INDEX_BATCH_SIZE);

  m_walker.setExcludedPaths(fullScan.excludedPaths);
  {
    std::error_code ec;
    batchedIndex.emplace_back(FileEventType::Modify, scan.path, fs::last_write_time(scan.path, ec), true,
                              std::nullopt);
  }

  m_walker.walk(scan.path, [&](const fs::directory_entry &entry) {
    std::error_code ec;
    reportProgress();
    bool const isDirectory = entry.is_directory(ec);
    batchedIndex.emplace_back(FileEventType::Modify, entry.path(), entry.last_write_time(ec), isDirectory,
                              file_indexer::fileSizeBytesFor(entry.path(), isDirectory));

    if (batchedIndex.size() >= INDEX_BATCH_SIZE) {
      m_writer->indexEvents(std::move(batchedIndex));
      batchedIndex = {};
      batchedIndex.reserve(INDEX_BATCH_SIZE);
    }
  });

  m_writer->indexEvents(std::move(batchedIndex));
}

IndexerScanner::IndexerScanner(const std::shared_ptr<DbWriter> &writer, const Scan &sc,
                               StatusCallback callback)
    : AbstractScanner(writer, sc, std::move(callback)) {}

void IndexerScanner::run() {
  start();

  bool failed = false;
  try {
    scan(m_scan);
  } catch (const std::exception &error) {
    flog::error() << "Caught exception during fullscan" << error.what();
    failed = true;
  }

  failed ? fail() : finish();
}

void IndexerScanner::interrupt() {
  setInterruptFlag();
  m_walker.stop();
}
