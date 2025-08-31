#include "scan-dispatcher.hpp"
#include "file-indexer.hpp"
#include <memory>

void ScanDispatcher::enqueue(const Scan& scan) {
  switch(scan.type) {
    case ScanType::Full:
    case ScanType::Incremental:
      m_indexerScanner->enqueue(scan);
  }
}

ScanDispatcher::ScanDispatcher(std::shared_ptr<IndexerScanner> indexerScanner):
  m_indexerScanner(indexerScanner),
  m_indexerThread([this](){ m_indexerScanner->run(); }) {}

ScanDispatcher::~ScanDispatcher() {
  m_indexerScanner->stop();
  m_indexerThread.join();
}
