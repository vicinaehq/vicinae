#pragma once
#include "file-indexer/scan.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/db-writer.hpp"
#include <atomic>
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
  std::shared_ptr<DbWriter> m_writer;
  EventCallback m_eventCallback;

  struct Element {
    Scan scan;
    std::unique_ptr<AbstractScanner> scanner;
  };

  std::mutex m_scannerMapMtx;
  std::map<int, Element> m_scannerMap;

  std::mutex m_collectorQueueMtx;
  std::queue<int> m_collectorQueue;

  std::thread m_collectorThread;
  std::condition_variable m_collectorCv;
  std::atomic<bool> m_running;

  void handleFinishedScan(int id, ScanStatus status);

public:
  ScanDispatcher(std::shared_ptr<DbWriter> writer);
  ~ScanDispatcher();

  void setEventCallback(EventCallback callback) { m_eventCallback = std::move(callback); }

  int enqueue(const Scan &scan);
  bool interrupt(int id);
  void interruptAll();

  // A vector instead of a lazy range because m_scannerMap is very mutable
  std::vector<std::pair<int, Scan>> scans();
};
