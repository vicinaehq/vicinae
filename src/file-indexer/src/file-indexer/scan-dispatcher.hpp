#pragma once
#include "file-indexer/scan.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/db-writer.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class ScanDispatcher {
public:
  using EventCallback = std::function<void(const ScanEvent &)>;

private:
  static constexpr size_t WORKER_COUNT = 2;
  static const constexpr std::chrono::seconds DEBOUNCE_QUIET{5};
  static const constexpr std::chrono::seconds DEBOUNCE_MAX_DELAY{30};

  struct Running {
    Scan scan;
    std::unique_ptr<AbstractScanner> scanner;
    std::chrono::steady_clock::time_point startedAt;
  };

  struct Pending {
    Scan scan;
    std::chrono::steady_clock::time_point deadline;
    std::chrono::steady_clock::time_point firstSeen;
  };

  std::shared_ptr<DbWriter> m_writer;
  EventCallback m_eventCallback;

  std::mutex m_workMtx;
  std::deque<std::pair<int, Scan>> m_ready;
  std::map<int, Running> m_running;
  std::condition_variable m_workCv;
  std::condition_variable m_idleCv;
  int m_nextScanId = 0;
  std::atomic<bool> m_alive;

  std::mutex m_pendingMtx;
  std::vector<Pending> m_pending;
  std::condition_variable m_pendingCv;

  std::vector<std::thread> m_workers;
  std::thread m_schedulerThread;

  void schedulerLoop();
  void workerLoop();
  void mergePendingLocked(const Scan &scan, std::chrono::steady_clock::time_point now);
  std::unique_ptr<AbstractScanner> makeScanner(int scanId, const Scan &scan, FileIndexerDatabase &readDb);

public:
  ScanDispatcher(std::shared_ptr<DbWriter> writer);
  ~ScanDispatcher();

  void setEventCallback(EventCallback callback) { m_eventCallback = std::move(callback); }

  int enqueue(const Scan &scan);

  void enqueueDebounced(const Scan &scan);

  bool interrupt(int id);
  void interruptAll();
  void clearPending();
  void waitUntilIdle();

  std::vector<std::pair<int, Scan>> scans();
};
