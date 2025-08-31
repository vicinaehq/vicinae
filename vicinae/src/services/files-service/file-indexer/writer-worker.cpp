#include "writer-worker.hpp"

namespace fs = std::filesystem;

void WriterWorker::stop() {
  m_alive = false;
  m_batchCv.notify_one();
}

void WriterWorker::run() {
  db = std::make_unique<FileIndexerDatabase>();

  while (m_alive) {
    std::deque<std::vector<std::filesystem::path>> batch;

    {
      std::unique_lock<std::mutex> lock(batchMutex);

      m_batchCv.wait(lock, [&]() { return !batchQueue.empty(); });
      batch = std::move(batchQueue);
      batchQueue.clear();
    }

    for (const auto &paths : batch) {
      batchWrite(paths);
    }
  }
}

void WriterWorker::batchWrite(const std::vector<fs::path> &paths) {
  // Writing is happening in the writerThread
  db->indexFiles(paths);
}

WriterWorker::WriterWorker(std::mutex &batchMutex, std::deque<std::vector<std::filesystem::path>> &batchQueue,
                           std::condition_variable &batchCv)
    : batchMutex(batchMutex), batchQueue(batchQueue), m_batchCv(batchCv) {}
