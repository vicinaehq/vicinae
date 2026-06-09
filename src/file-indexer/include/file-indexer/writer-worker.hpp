#pragma once
#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>
#include "file-indexer/util.hpp"
#include "file-indexer/db-writer.hpp"

class WriterWorker : file_indexer::NonCopyable {
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
