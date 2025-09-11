#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include <atomic>
#include <map>
#include <memory>
#include <queue>

class ScanDispatcher {
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
  ScanDispatcher();
  ~ScanDispatcher();
  int enqueue(const Scan &scan);
};
