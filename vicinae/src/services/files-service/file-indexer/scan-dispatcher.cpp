#include "scan-dispatcher.hpp"
#include "services/files-service/file-indexer/indexer-scanner.hpp"
#include "services/files-service/file-indexer/incremental-scanner.hpp"
#include <map>
#include <memory>
#include <mutex>

void ScanDispatcher::handleFinishedScan(int id, ScanStatus status) {
  {
    std::scoped_lock l(m_collectorQueueMtx);
    m_collectorQueue.push(id);
  }
  m_collectorCv.notify_one();
}

int ScanDispatcher::enqueue(const Scan &scan) {
  static int idCounter;

  int scanId = idCounter;
  idCounter++;

  auto handler = [this, scanId](ScanStatus status) { handleFinishedScan(scanId, status); };

  {
    std::scoped_lock l(m_scannerMapMtx);

    switch (scan.type) {
    case ScanType::Full:
      m_scannerMap[scanId] = {scan, std::make_unique<IndexerScanner>(scan, handler)};
      break;
    case ScanType::Incremental:
      m_scannerMap[scanId] = {scan, std::make_unique<IncrementalScanner>(scan, handler)};
      break;
    }
  }
  return scanId;
}

ScanDispatcher::ScanDispatcher() {
  m_running = true;

  m_collectorThread = std::thread([this]() {
    while (true) {
      std::unique_lock lock(m_collectorQueueMtx);
      m_collectorCv.wait(lock, [&]() { return !m_collectorQueue.empty() || !m_running; });

      if (!m_running) break;

      int id;
      {
        std::scoped_lock l(m_collectorQueueMtx);
        id = m_collectorQueue.front();
        m_collectorQueue.pop();
      }
      {
        std::scoped_lock l(m_scannerMapMtx);
        m_scannerMap[id].scanner->join();
        m_scannerMap.erase(id);
      }
    }
  });
}

ScanDispatcher::~ScanDispatcher() {
  m_running = false;

  {
    std::scoped_lock l(m_scannerMapMtx);

    for (auto& [id, element]: m_scannerMap) {
      {
        element.scanner->interrupt();
        element.scanner->join();
      }
    }

    m_scannerMap.clear();
  }

  m_collectorCv.notify_one();
  m_collectorThread.join();
}
