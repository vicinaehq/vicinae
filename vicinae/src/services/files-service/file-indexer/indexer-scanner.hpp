#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/writer-worker.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"

class IndexerScanner : public AbstractScanner, public NonCopyable {
private:
  static constexpr size_t INDEX_BATCH_SIZE = 10'000;
  static constexpr size_t MAX_PENDING_BATCH_COUNT = 10;
  static constexpr size_t BACKPRESSURE_WAIT_MS = 100;

  std::deque<std::vector<FileEvent>> m_writeBatches;
  std::mutex m_batchMutex;
  std::condition_variable m_batchCv;

  std::unique_ptr<WriterWorker> m_writerWorker;
  std::thread m_writerThread;

  std::thread m_scanThread;

  void scan(const Scan &scan);
  void enqueueBatch(const std::vector<FileEvent> &paths);

public:
  IndexerScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, FinishCallback callback);
  ~IndexerScanner() = default;

  void interrupt() override;
  void join() override;
};
