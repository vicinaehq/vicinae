#include "file-indexer/indexer-scanner.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/filesystem-walker.hpp"
#include "file-indexer/log.hpp"
#include <chrono>
#include <utility>

namespace fs = std::filesystem;

void IndexerScanner::enqueueBatch(const std::vector<FileEvent> &paths) {
  bool shouldWait = true;

  while (shouldWait) {
    if (!m_alive) return;

    {
      std::scoped_lock const lock(m_batchMutex);

      /**
       * handle backpressure by waiting if too many batches are queued
       */
      shouldWait = m_writeBatches.size() >= MAX_PENDING_BATCH_COUNT;
      if (!shouldWait) { m_writeBatches.emplace_back(paths); }
    }

    if (shouldWait) {
      // flog::debug() << "Handling backpressure: too many batched";
      std::this_thread::sleep_for(std::chrono::milliseconds(BACKPRESSURE_WAIT_MS));
    }
  }

  m_batchCv.notify_one();
}

void IndexerScanner::scan(const Scan &scan) {
  std::vector<FileEvent> batchedIndex;

  m_walker.setVerbose();
  m_walker.setExcludedPaths(scan.excludedPaths);
  m_walker.walk(scan.path, [&](const fs::directory_entry &entry) {
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

IndexerScanner::IndexerScanner(const std::shared_ptr<DbWriter> &writer, const Scan &sc,
                               FinishCallback callback)
    : AbstractScanner(writer, sc, std::move(callback)) {
  m_writerWorker = std::make_unique<WriterWorker>(writer, m_batchMutex, m_writeBatches, m_batchCv);
  m_writerThread = std::thread([&]() { m_writerWorker->run(); });

  m_scanThread = std::thread([this, sc]() {
    start(sc);

    bool failed = false;
    try {
      scan(sc);
    } catch (const std::exception &error) {
      flog::error() << "Caught exception during fullscan" << error.what();
      failed = true;
    }

    m_writerWorker->stop();
    m_writerThread.join();

    failed ? fail() : finish();
  });
}

void IndexerScanner::interrupt() {
  setInterruptFlag();
  m_alive = false;
  m_walker.stop();
  m_batchCv.notify_all();
  m_writerWorker->stop();
}

void IndexerScanner::join() { m_scanThread.join(); }
