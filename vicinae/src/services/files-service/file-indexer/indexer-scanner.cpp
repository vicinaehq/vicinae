#include "indexer-scanner.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include <qlogging.h>

namespace fs = std::filesystem;

void IndexerScanner::stop(bool regurgitate) {
  AbstractScanner::stop(regurgitate);

  m_writerThread.join();
}

void IndexerScanner::enqueueBatch(const std::vector<std::filesystem::path> &paths) {
  bool shouldWait = true;

  while (shouldWait) {
    {
      std::lock_guard lock(m_batchMutex);

      /**
       * handle backpressure by waiting if too many batches are queued
       */
      shouldWait = m_writeBatches.size() >= MAX_PENDING_BATCH_COUNT;
      if (!shouldWait) { m_writeBatches.emplace_back(paths); }
    }

    if (shouldWait) {
      qDebug() << "Handling backpressure: too many batched";
      std::this_thread::sleep_for(std::chrono::milliseconds(BACKPRESSURE_WAIT_MS));
    }
  }

  m_batchCv.notify_one();
}

void IndexerScanner::scan(const std::filesystem::path &root) {
  std::vector<fs::path> batchedIndex;
  FileSystemWalker walker;

  walker.walk(root, [&](const fs::directory_entry &entry) {
    batchedIndex.emplace_back(entry);

    if (batchedIndex.size() > INDEX_BATCH_SIZE) {
      enqueueBatch(batchedIndex);
      batchedIndex.clear();
    }
  });

  enqueueBatch(batchedIndex);
}

void IndexerScanner::run() {
  AbstractScanner::run();

  m_db = std::make_unique<FileIndexerDatabase>();
  m_writerWorker = std::make_unique<WriterWorker>(m_batchMutex, m_writeBatches, m_batchCv);
  m_writerThread = std::thread([&]() { m_writerWorker->run(); });

  while (true) {
    const auto expected = awaitScan();
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
    scan(sc.path);
    m_db->updateScanStatus(scanRecord.id, FileIndexerDatabase::ScanStatus::Finished);

    finishScan(sc);
  }
}
