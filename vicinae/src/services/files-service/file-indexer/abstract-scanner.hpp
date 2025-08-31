#pragma once
#include "services/files-service/file-indexer/scan.hpp"

#include <condition_variable>
#include <expected>
#include <mutex>
#include <queue>
#include <variant>
class AbstractScanner {
  std::atomic<bool> m_alive;
  std::queue<Scan> m_scans;
  std::mutex m_scanLock;
  std::condition_variable m_scanCv;

public:
  std::expected<Scan, bool> awaitScan();
  void enqueue(const Scan& scan);
  virtual void run();
  virtual void stop();
};
