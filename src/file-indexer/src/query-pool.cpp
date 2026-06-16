#include "file-indexer/query-pool.hpp"
#include <utility>

QueryPool::QueryPool(size_t workerCount) {
  m_workers.reserve(workerCount);
  for (size_t i = 0; i < workerCount; ++i) {
    m_workers.emplace_back([this] { workerLoop(); });
  }
}

QueryPool::~QueryPool() {
  {
    std::scoped_lock const lock(m_mtx);
    m_stop = true;
  }
  m_cv.notify_all();
  for (auto &worker : m_workers) {
    if (worker.joinable()) worker.join();
  }
}

void QueryPool::submit(Job job) {
  std::unique_lock lock(m_mtx);

  if (m_queue.size() >= MAX_PENDING) {
    Job stale = std::move(m_queue.front());
    m_queue.pop_front();
    lock.unlock();
    // we still need to reply to evicted job
    stale.onResult({});
    lock.lock();
  }

  m_queue.emplace_back(std::move(job));
  m_cv.notify_one();
}

void QueryPool::workerLoop() {
  FileIndexerQueryEngine engine;

  while (true) {
    Job job;

    {
      std::unique_lock lock(m_mtx);
      m_cv.wait(lock, [this] { return m_stop || !m_queue.empty(); });
      if (m_stop && m_queue.empty()) return;
      job = std::move(m_queue.front());
      m_queue.pop_front();
    }

    job.onResult(engine.query(job.text, job.limit));
  }
}
