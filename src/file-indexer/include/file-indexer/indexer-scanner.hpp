#pragma once
#include "file-indexer/util.hpp"
#include "file-indexer/writer-worker.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/filesystem-walker.hpp"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <thread>

class IndexerScanner : public AbstractScanner, public file_indexer::NonCopyable {
private:
  static constexpr size_t INDEX_BATCH_SIZE = 10'000;
  static constexpr size_t MAX_PENDING_BATCH_COUNT = 10;
  static constexpr size_t BACKPRESSURE_WAIT_MS = 100;

  std::deque<std::vector<FileEvent>> m_writeBatches;
  std::mutex m_batchMutex;
  std::condition_variable m_batchCv;
  std::atomic<bool> m_alive = true;

  FileSystemWalker m_walker;

  std::unique_ptr<WriterWorker> m_writerWorker;
  std::thread m_writerThread;

  std::thread m_scanThread;

  void scan(const Scan &scan);
  void enqueueBatch(const std::vector<FileEvent> &paths);

public:
  IndexerScanner(const std::shared_ptr<DbWriter> &writer, const Scan &scan, FinishCallback callback);
  ~IndexerScanner() = default;

  void interrupt() override;
  void join() override;
};
