#pragma once
#include <atomic>
#include <condition_variable>
#include <expected>
#include <functional>
#include <filesystem>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/io-pacer.hpp"

class DbWriter {
public:
  using Work = std::function<void(FileIndexerDatabase &)>;

private:
  static constexpr size_t MAX_PENDING_BULK_WRITES = 8;

  struct QueuedWork {
    Work work;
    bool bounded;
  };

  std::mutex m_queueMtx;
  std::queue<QueuedWork> m_queue;
  size_t m_pendingBulkWrites = 0;

  std::atomic<bool> m_active = true;
  std::atomic<bool> m_vocabRebuildQueued = false;

  std::condition_variable m_updateSignal;
  std::condition_variable m_notFull;

  std::unique_ptr<FileIndexerDatabase> m_db;

  file_indexer::IoPacer m_pacer{"/proc/pressure/io", 1};

  std::thread m_workerThread;

  void listen();

public:
  DbWriter();
  ~DbWriter();

  void submit(Work work, bool bounded = false);

  void updateScanStatus(int scanId, ScanStatus status);
  void finalizeScan(int scanId, ScanStatus status, int64_t indexedFileCount);

  void setScanError(int scanId, const std::string &error);
  std::expected<FileIndexerDatabase::ScanRecord, std::string> createScan(const std::filesystem::path &path,
                                                                         ScanType type);

  void indexFiles(std::vector<std::filesystem::path> paths);
  void deleteIndexedFiles(std::vector<std::filesystem::path> paths,
                          std::function<void()> onComplete = nullptr);
  void deleteAllIndexedFiles(std::function<void()> onComplete = nullptr);
  void compact(std::function<void()> onComplete = nullptr);

  void rebuildSpellfixVocabulary();

  void indexEvents(std::vector<FileEvent> events);
};
