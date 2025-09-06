#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include <map>
#include <memory>
#include <thread>
#include <utility>

class ScanDispatcher {
  struct Element {
    std::shared_ptr<AbstractScanner> scanner;
    std::thread thread;
    bool alive;
  };

  std::map<ScanType, Element> m_scannerMap;

public:
  void enableAll();

  void enqueue(const Scan &scan);
  void enable(ScanType type);
  void disable(ScanType type, bool regurgitate);

  ScanDispatcher(std::map<ScanType, std::shared_ptr<AbstractScanner>> scannerMap);
  ~ScanDispatcher();
};
