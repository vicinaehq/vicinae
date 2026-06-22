#include "file-indexer/db-writer.hpp"
#include "file-indexer/background-thread.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/log.hpp"
#include <future>

void DbWriter::listen() {
  file_indexer::setBackgroundThreadPriority();
  m_db = std::make_unique<FileIndexerDatabase>();
  if (!m_db->isOpen()) {
    flog::error() << "File indexer writer database is not open, queued writes will fail";
  }

  while (true) {
    std::unique_lock lock(m_queueMtx);
    m_updateSignal.wait(lock, [&]() { return !m_queue.empty() || !m_active; });

    if (m_queue.empty() && !m_active) break;

    QueuedWork const work = std::move(m_queue.front());
    m_queue.pop();
    lock.unlock();

    m_pacer.checkpoint();
    work.work(*m_db);

    if (work.bounded) {
      {
        std::scoped_lock const l(m_queueMtx);
        --m_pendingBulkWrites;
      }
      m_notFull.notify_one();
    }
  }
}

void DbWriter::submit(Work work, bool bounded) {
  {
    std::unique_lock lock(m_queueMtx);

    if (bounded) {
      m_notFull.wait(lock, [&]() { return m_pendingBulkWrites < MAX_PENDING_BULK_WRITES || !m_active; });
      ++m_pendingBulkWrites;
    }

    m_queue.push({std::move(work), bounded});
  }
  m_updateSignal.notify_one();
}

DbWriter::DbWriter() {
  m_workerThread = std::thread([this]() { listen(); });
}

DbWriter::~DbWriter() {
  m_active = false;
  m_updateSignal.notify_one();
  m_notFull.notify_all();
  m_workerThread.join();
}

void DbWriter::setScanError(int scanId, const std::string &error) {
  submit([scanId, error = std::move(error)](FileIndexerDatabase &db) { db.setScanError(scanId, error); });
}

void DbWriter::finalizeScan(int scanId, ScanStatus status, int64_t indexedFileCount) {
  submit([scanId, status, indexedFileCount](FileIndexerDatabase &db) {
    db.finalizeScan(scanId, status, indexedFileCount);
  });
}

void DbWriter::updateScanStatus(int scanId, ScanStatus status) {
  submit([scanId, status](FileIndexerDatabase &db) { db.updateScanStatus(scanId, status); });
}

std::expected<FileIndexerDatabase::ScanRecord, std::string>
DbWriter::createScan(const std::filesystem::path &path, ScanType type) {
  std::promise<std::expected<FileIndexerDatabase::ScanRecord, std::string>> result;
  auto future = result.get_future();

  submit([&result, &path, type](FileIndexerDatabase &db) { result.set_value(db.createScan(path, type)); });

  return future.get();
}

void DbWriter::indexFiles(std::vector<std::filesystem::path> paths) {
  submit([paths = std::move(paths)](FileIndexerDatabase &db) { db.indexFiles(paths); }, true);
}

void DbWriter::deleteIndexedFiles(std::vector<std::filesystem::path> paths,
                                  std::function<void()> onComplete) {
  submit([paths = std::move(paths), onComplete = std::move(onComplete)](FileIndexerDatabase &db) {
    db.deleteIndexedFiles(paths);
    if (onComplete) { onComplete(); }
  });
}

void DbWriter::deleteAllIndexedFiles(std::function<void()> onComplete) {
  submit([onComplete = std::move(onComplete)](FileIndexerDatabase &db) {
    db.deleteAllIndexedFiles();
    if (onComplete) { onComplete(); }
  });
}

void DbWriter::compact(std::function<void()> onComplete) {
  submit([onComplete = std::move(onComplete)](FileIndexerDatabase &db) {
    db.compact();
    if (onComplete) { onComplete(); }
  });
}

void DbWriter::indexEvents(std::vector<FileEvent> events) {
  submit([events = std::move(events)](FileIndexerDatabase &db) { db.indexEvents(events); }, true);
}

void DbWriter::rebuildSpellfixVocabulary() {
  if (m_vocabRebuildQueued.exchange(true)) return;

  submit([this](FileIndexerDatabase &db) {
    m_vocabRebuildQueued = false;
    db.rebuildSpellfixVocabulary();
  });
}
