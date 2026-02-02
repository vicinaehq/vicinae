#include "indexer-scanner.hpp"
#include "abstract-scanner.hpp"
#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include <QDebug>

namespace fs = std::filesystem;

void IndexerScanner::enqueueBatch(const std::vector<FileEvent> &paths) {
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

void IndexerScanner::scan(const Scan &scan) {
  std::vector<FileEvent> batchedIndex;
  FileSystemWalker walker;

  walker.setVerbose();
  walker.setExcludedPaths(scan.excludedPaths);
  walker.walk(scan.path, [&](const fs::directory_entry &entry) {
    std::error_code ec;
    // In case of error, returns file_time_time::min() - erroring entries deserve a bad relevance score anyway
    batchedIndex.emplace_back(FileEventType::Modify, entry.path(), entry.last_write_time(ec));

    if (batchedIndex.size() > INDEX_BATCH_SIZE) {
      enqueueBatch(batchedIndex);
      batchedIndex.clear();
    }
  });

  enqueueBatch(batchedIndex);
}

IndexerScanner::IndexerScanner(std::shared_ptr<DbWriter> writer, const Scan &sc, FinishCallback callback)
    : AbstractScanner(writer, sc, callback) {
  m_writerWorker = std::make_unique<WriterWorker>(writer, m_batchMutex, m_writeBatches, m_batchCv);
  m_writerThread = std::thread([&]() { m_writerWorker->run(); });

  m_scanThread = std::thread([this, sc]() {
    start(sc);

    try {
      scan(sc);
      m_writerWorker->stop();
      finish();
    } catch (const std::exception &error) {
      qCritical() << "Caught exception during fullscan" << error.what();
      fail();
    }
  });
}

void IndexerScanner::interrupt() {
  setInterruptFlag();
  m_writerWorker->stop();
}

void IndexerScanner::join() {
  m_writerThread.join();
  m_scanThread.join();
}
