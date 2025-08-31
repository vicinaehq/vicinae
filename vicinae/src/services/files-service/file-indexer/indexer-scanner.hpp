#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/writer-worker.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"

class IndexerScanner : public AbstractScanner, public NonCopyable {
private:
  static constexpr size_t INDEX_BATCH_SIZE = 10'000;
  static constexpr size_t MAX_PENDING_BATCH_COUNT = 10;
  static constexpr size_t BACKPRESSURE_WAIT_MS = 100;
  std::unique_ptr<FileIndexerDatabase> m_db;

  std::deque<std::vector<std::filesystem::path>> m_writeBatches;
  std::mutex m_batchMutex;
  std::condition_variable m_batchCv;

  std::unique_ptr<WriterWorker> m_writerWorker;
  std::thread m_writerThread;

  void scan(const std::filesystem::path &path);
  void enqueueBatch(const std::vector<std::filesystem::path> &paths);

public:
  void run() override;
  void stop() override;
};
