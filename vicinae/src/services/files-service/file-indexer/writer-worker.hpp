#include <atomic>
#include <deque>
#include "common.hpp"
#include "services/files-service/file-indexer/db-writer.hpp"

class WriterWorker : public NonCopyable {
  std::shared_ptr<DbWriter> m_writer;

  std::mutex &batchMutex;
  std::deque<std::vector<std::filesystem::path>> &batchQueue;
  std::condition_variable &m_batchCv;
  std::atomic<bool> m_alive = true;
  std::atomic<bool> m_isWorking = false;

  void batchWrite(const std::vector<std::filesystem::path> &paths);

public:
  void run();
  void stop();
  bool isWorking() const { return m_isWorking; }

  WriterWorker(std::shared_ptr<DbWriter> writer, std::mutex &batchMutex,
               std::deque<std::vector<std::filesystem::path>> &batchQueue, std::condition_variable &batchCv);
};
