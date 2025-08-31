#include "scan-dispatcher.hpp"
#include "file-indexer.hpp"
#include <map>
#include <memory>

void ScanDispatcher::enqueue(const Scan &scan) { m_scannerMap[scan.type].first->enqueue(scan); }

ScanDispatcher::ScanDispatcher(std::map<ScanType, std::shared_ptr<AbstractScanner>> scannerMap) {
  for (const auto &[type, scanner] : scannerMap) {
    m_scannerMap[type] = {scanner, std::thread([scanner]() { scanner->run(); })};
  }
}

ScanDispatcher::~ScanDispatcher() {
  for (auto &[type, pair] : m_scannerMap) {
    pair.first->stop();
    pair.second.join();
  }
}
