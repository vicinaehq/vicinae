#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/indexer-scanner.hpp"
#include <memory>

class ScanDispatcher {
  std::shared_ptr<IndexerScanner> m_indexerScanner;
  std::thread m_indexerThread;

public:
  void enqueue(const Scan& scan);

  ScanDispatcher(std::shared_ptr<IndexerScanner> indexerScanner);
  ~ScanDispatcher();
};
