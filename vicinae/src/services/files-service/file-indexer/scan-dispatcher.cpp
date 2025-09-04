#include "scan-dispatcher.hpp"
#include <map>
#include <memory>

void ScanDispatcher::enqueue(const Scan &scan) { m_scannerMap[scan.type].scanner->enqueue(scan); }

ScanDispatcher::ScanDispatcher(std::map<ScanType, std::shared_ptr<AbstractScanner>> scannerMap) {
  for (const auto &[type, scanner] : scannerMap) {
    m_scannerMap[type] = {scanner, {}, false};
  }
}

void ScanDispatcher::enableAll() {
  for (const auto &[type, scanner] : m_scannerMap) {
    enable(type);
  }
}

void ScanDispatcher::enable(ScanType type) {
  auto &element = m_scannerMap[type];
  if (element.alive) return;

  auto scannerPtr = element.scanner;
  element.thread = std::thread([scannerPtr]() { scannerPtr->run(); });
  element.alive = true;
}

void ScanDispatcher::disable(ScanType type, bool regurgitate) {
  auto &element = m_scannerMap[type];
  if (!element.alive) return;

  element.scanner->stop(regurgitate);
  element.thread.join();
  element.alive = false;
}

ScanDispatcher::~ScanDispatcher() {
  for (auto &[type, pair] : m_scannerMap) {
    pair.scanner->stop(false);
    pair.thread.join();
  }
}
