#include <atomic>
#include <deque>
#include "common.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"

class WriterWorker : public NonCopyable {
  std::unique_ptr<FileIndexerDatabase> db;
  std::mutex &batchMutex;
  std::deque<std::vector<std::filesystem::path>> &batchQueue;
  std::condition_variable &m_batchCv;
  std::atomic<bool> m_alive = true;

  void batchWrite(const std::vector<std::filesystem::path> &paths);

public:
  void run();
  void stop();

  WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
               std::condition_variable &batchCv);
};
