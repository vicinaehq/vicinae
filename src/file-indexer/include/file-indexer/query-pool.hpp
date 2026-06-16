#pragma once
#include "file-indexer/file-indexer-query-engine.hpp"
#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class QueryPool {
public:
  struct Job {
    std::string text;
    int limit;
    std::function<void(std::vector<IndexerFileResult>)> onResult;
  };

  explicit QueryPool(size_t workerCount);
  ~QueryPool();

  QueryPool(const QueryPool &) = delete;
  QueryPool &operator=(const QueryPool &) = delete;

  void submit(Job job);

private:
  void workerLoop();

  static constexpr size_t MAX_PENDING = 8;

  std::vector<std::thread> m_workers;
  std::deque<Job> m_queue;
  std::mutex m_mtx;
  std::condition_variable m_cv;
  bool m_stop = false;
};
