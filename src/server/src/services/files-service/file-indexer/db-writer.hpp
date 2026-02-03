#pragma once
#include "utils/utils.hpp"
#include <queue>
#include <vector>
#include <filesystem>
#include "services/files-service/file-indexer/file-indexer-db.hpp"

class DbWriter {
public:
  using Work = std::function<void(FileIndexerDatabase &)>;

private:
  std::mutex m_queueMtx;
  std::queue<Work> m_queue;

  std::atomic<bool> m_active = true;
  std::condition_variable m_updateSignal;

  std::unique_ptr<FileIndexerDatabase> m_db;

  std::thread m_workerThread;

  void listen();

public:
  DbWriter();
  ~DbWriter();

  // Use with std::move to avoid copies
  void submit(Work work);

  // Utility functions
  void updateScanStatus(int scanId, ScanStatus status);

  void setScanError(int scanId, const QString &error);
  std::expected<FileIndexerDatabase::ScanRecord, QString> createScan(const std::filesystem::path &path,
                                                                    ScanType type);

  // Receive by value because `paths` could mutate while the work is waiting in queue
  void indexFiles(std::vector<std::filesystem::path> paths);
  void deleteIndexedFiles(std::vector<std::filesystem::path> paths);
  void deleteAllIndexedFiles(std::function<void()> onComplete = nullptr);

  void indexEvents(std::vector<FileEvent> events);
};
