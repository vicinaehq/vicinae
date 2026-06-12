#pragma once
#include "file-indexer/scan.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/db-writer.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ScanDispatcher {
public:
  using EventCallback = std::function<void(const ScanEvent &)>;

private:
  static constexpr std::chrono::seconds DEBOUNCE_QUIET{5};
  static constexpr std::chrono::seconds DEBOUNCE_MAX_DELAY{30};

  std::shared_ptr<DbWriter> m_writer;
  EventCallback m_eventCallback;

  struct Element {
    Scan scan;
    std::unique_ptr<AbstractScanner> scanner;
  };

  struct Pending {
    Scan scan;
    std::chrono::steady_clock::time_point deadline;
    std::chrono::steady_clock::time_point firstSeen;
  };

  std::mutex m_scannerMapMtx;
  std::map<int, Element> m_scannerMap;

  std::mutex m_collectorQueueMtx;
  std::queue<int> m_collectorQueue;

  std::thread m_collectorThread;
  std::condition_variable m_collectorCv;
  std::atomic<bool> m_running;

  std::mutex m_pendingMtx;
  std::vector<Pending> m_pending;
  std::condition_variable m_pendingCv;
  std::thread m_schedulerThread;

  void handleFinishedScan(int id, ScanStatus status);
  void schedulerLoop();

public:
  ScanDispatcher(std::shared_ptr<DbWriter> writer);
  ~ScanDispatcher();

  void setEventCallback(EventCallback callback) { m_eventCallback = std::move(callback); }

  int enqueue(const Scan &scan);

  // coalesces identical requests: starts after DEBOUNCE_QUIET of silence, at
  // most DEBOUNCE_MAX_DELAY after the first request
  void enqueueDebounced(const Scan &scan);

  bool interrupt(int id);
  void interruptAll();

  // A vector instead of a lazy range because m_scannerMap is very mutable
  std::vector<std::pair<int, Scan>> scans();
};
