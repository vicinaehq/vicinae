#include <atomic>
#include <deque>
#include "common/types.hpp"
#include "services/files-service/file-indexer/db-writer.hpp"

class WriterWorker : NonCopyable {
  std::shared_ptr<DbWriter> m_writer;

  std::mutex &batchMutex;
  std::deque<std::vector<FileEvent>> &batchQueue;
  std::condition_variable &m_batchCv;
  std::atomic<bool> m_alive = true;
  std::atomic<bool> m_isWorking = false;

  void batchWrite(std::vector<FileEvent> paths);

public:
  void run();
  void stop();
  bool isWorking() const { return m_isWorking; }

  WriterWorker(std::shared_ptr<DbWriter> writer, std::mutex &batchMutex,
               std::deque<std::vector<FileEvent>> &batchQueue, std::condition_variable &batchCv);
};
