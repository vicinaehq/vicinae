#include "writer-worker.hpp"

namespace fs = std::filesystem;

void WriterWorker::stop() {
  m_alive = false;
  m_batchCv.notify_one();
}

void WriterWorker::run() {
  while (m_alive) {
    std::deque<std::vector<FileEvent>> batch;

    {
      std::unique_lock<std::mutex> lock(batchMutex);

      m_batchCv.wait(lock, [&]() { return !batchQueue.empty(); });
      batch = std::move(batchQueue);
      batchQueue.clear();
    }

    m_isWorking = true;
    for (const auto &paths : batch) {
      batchWrite(std::move(paths));
    }
    m_isWorking = false;
  }
}

void WriterWorker::batchWrite(std::vector<FileEvent> paths) {
  // Writing is happening in the writerThread
  m_writer->indexEvents(std::move(paths));
}

WriterWorker::WriterWorker(std::shared_ptr<DbWriter> writer, std::mutex &batchMutex,
                           std::deque<std::vector<FileEvent>> &batchQueue, std::condition_variable &batchCv)
    : m_writer(writer), batchMutex(batchMutex), batchQueue(batchQueue), m_batchCv(batchCv) {}
