#pragma once
#include "services/files-service/file-indexer/scan.hpp"

#include <condition_variable>
#include <expected>
#include <mutex>
#include <deque>
#include <set>

class AbstractScanner {
  std::atomic<bool> m_alive;
  std::deque<Scan> m_queuedScans;
  std::set<Scan> m_aliveScans;
  std::mutex m_scanLock;
  std::condition_variable m_scanCv;

public:
  std::expected<Scan, bool> awaitScan();
  void finishScan(const Scan &scan);

  void enqueue(const Scan &scan);

  virtual void run();
  virtual void stop(bool regurgitate);
};
