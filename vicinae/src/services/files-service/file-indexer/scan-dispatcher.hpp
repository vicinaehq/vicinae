#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include <map>
#include <memory>
#include <utility>

class ScanDispatcher {
  std::map<ScanType, std::pair<std::shared_ptr<AbstractScanner>, std::thread>> m_scannerMap;

public:
  void enqueue(const Scan &scan);

  ScanDispatcher(std::map<ScanType, std::shared_ptr<AbstractScanner>> scannerMap);
  ~ScanDispatcher();
};
