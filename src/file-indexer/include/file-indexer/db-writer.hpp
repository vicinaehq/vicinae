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

class DbWriter {
public:
  using Work = std::function<void(FileIndexerDatabase &)>;

private:
  // Backpressure cap on in-flight bulk writes (indexEvents/indexFiles). Producers
  // block once this many are queued so the file walk cannot outrun the writer thread.
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

  std::thread m_workerThread;

  void listen();

public:
  DbWriter();
  ~DbWriter();

  // Use with std::move to avoid copies. Pass bounded=true for high-volume writes so
  // the call blocks under backpressure; control-plane submits must stay unbounded.
  void submit(Work work, bool bounded = false);

  // Utility functions
  void updateScanStatus(int scanId, ScanStatus status);

  void setScanError(int scanId, const std::string &error);
  std::expected<FileIndexerDatabase::ScanRecord, std::string> createScan(const std::filesystem::path &path,
                                                                         ScanType type);

  // Receive by value because `paths` could mutate while the work is waiting in queue
  void indexFiles(std::vector<std::filesystem::path> paths);
  void deleteIndexedFiles(std::vector<std::filesystem::path> paths);
  void deleteAllIndexedFiles(std::function<void()> onComplete = nullptr);
  void compact(std::function<void()> onComplete = nullptr);

  // Coalescing: scheduling while a rebuild is already queued is a no-op, so a
  // burst of scan completions results in a single rebuild.
  void rebuildSpellfixVocabulary();

  void indexEvents(std::vector<FileEvent> events);
};
