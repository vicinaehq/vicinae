#include "db-writer.hpp"
#include "file-indexer-db.hpp"
#include <QtLogging>

void DbWriter::listen() {
  m_db = std::make_unique<FileIndexerDatabase>();

  while (true) {
    std::unique_lock lock(m_queueMtx);
    m_updateSignal.wait(lock, [&]() { return !m_queue.empty() || !m_active; });

    if (m_queue.empty() && !m_active) break;

    Work work = std::move(m_queue.front());
    m_queue.pop();
    lock.unlock();

    work(*m_db);
  }
}

void DbWriter::submit(Work work) {
  {
    std::scoped_lock l(m_queueMtx);
    m_queue.push(std::move(work));
  }
  m_updateSignal.notify_one();
}

DbWriter::DbWriter() {
  m_workerThread = std::thread([this]() { listen(); });
}

DbWriter::~DbWriter() {
  m_active = false;
  m_updateSignal.notify_one();
  m_workerThread.join();
}

void DbWriter::setScanError(int scanId, const QString &error) {
  submit([scanId, error = std::move(error)](FileIndexerDatabase &db) { db.setScanError(scanId, error); });
}

void DbWriter::updateScanStatus(int scanId, ScanStatus status) {
  submit([scanId, status](FileIndexerDatabase &db) { db.updateScanStatus(scanId, status); });
}

tl::expected<FileIndexerDatabase::ScanRecord, QString> DbWriter::createScan(const std::filesystem::path &path,
                                                                            ScanType type) {
  // TODO: Fragile lifetime issues
  // TODO: Proritize record creation over normal writes

  std::promise<tl::expected<FileIndexerDatabase::ScanRecord, QString>> result;
  auto future = result.get_future();

  submit([&result, &path, type](FileIndexerDatabase &db) { result.set_value(db.createScan(path, type)); });

  return future.get();
}

void DbWriter::indexFiles(std::vector<std::filesystem::path> paths) {
  submit([paths = std::move(paths)](FileIndexerDatabase &db) { db.indexFiles(paths); });
}

void DbWriter::deleteIndexedFiles(std::vector<std::filesystem::path> paths) {
  submit([paths = std::move(paths)](FileIndexerDatabase &db) { db.deleteIndexedFiles(paths); });
}

void DbWriter::deleteAllIndexedFiles(std::function<void()> onComplete) {
  submit([onComplete = std::move(onComplete)](FileIndexerDatabase &db) {
    db.deleteAllIndexedFiles();
    if (onComplete) { onComplete(); }
  });
}

void DbWriter::indexEvents(std::vector<FileEvent> events) {
  submit([events = std::move(events)](FileIndexerDatabase &db) { db.indexEvents(events); });
}
