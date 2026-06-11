#include "file-indexer/scan-dispatcher.hpp"
#include "file-indexer/indexer-scanner.hpp"
#include "file-indexer/incremental-scanner.hpp"
#include "file-indexer/log.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <ranges>
#include <utility>

void ScanDispatcher::handleFinishedScan(int id, ScanStatus status) {
  {
    std::scoped_lock const l(m_collectorQueueMtx);
    m_collectorQueue.push(id);
  }
  m_collectorCv.notify_one();
}

int ScanDispatcher::enqueue(const Scan &scan) {
  flog::info() << "Enqueuing scan for " << scan.path << "\n";
  static int idCounter;

  int const scanId = idCounter;
  idCounter++;

  auto handler = [this, scanId, scan](ScanStatus status, size_t processedCount) {
    if (scan.notify && m_eventCallback) {
      m_eventCallback({.scanId = scanId,
                       .type = scan.type,
                       .status = status,
                       .entrypoint = scan.path,
                       .processedFileCount = processedCount});
    }
    if (status != ScanStatus::Started) { handleFinishedScan(scanId, status); }
  };

  {
    std::scoped_lock const l(m_scannerMapMtx);

    switch (scan.type) {
    case ScanType::Full:
      m_scannerMap[scanId] = {scan, std::make_unique<IndexerScanner>(m_writer, scan, handler)};
      break;
    case ScanType::Incremental:
      m_scannerMap[scanId] = {scan, std::make_unique<IncrementalScanner>(m_writer, scan, handler)};
      break;
    }
  }
  return scanId;
}

void ScanDispatcher::enqueueDebounced(const Scan &scan) {
  auto const now = std::chrono::steady_clock::now();
  {
    std::scoped_lock const l(m_pendingMtx);
    auto it = std::ranges::find(m_pending, scan, &Pending::scan);

    if (it != m_pending.end()) {
      it->deadline = std::min(now + DEBOUNCE_QUIET, it->firstSeen + DEBOUNCE_MAX_DELAY);
    } else {
      m_pending.emplace_back(scan, now + DEBOUNCE_QUIET, now);
    }
  }
  m_pendingCv.notify_one();
}

void ScanDispatcher::schedulerLoop() {
  std::unique_lock lock(m_pendingMtx);

  while (m_running) {
    if (m_pending.empty()) {
      m_pendingCv.wait(lock, [&]() { return !m_pending.empty() || !m_running; });
      continue;
    }

    auto const next = std::ranges::min_element(m_pending, {}, &Pending::deadline)->deadline;
    if (m_pendingCv.wait_until(lock, next, [&]() { return !m_running.load(); })) break;

    auto const now = std::chrono::steady_clock::now();
    std::vector<Scan> due;

    due.reserve(m_pending.size());
    std::erase_if(m_pending, [&](const Pending &pending) {
      if (pending.deadline > now) return false;
      due.emplace_back(pending.scan);
      return true;
    });

    lock.unlock();
    for (const auto &scan : due) {
      if (!m_running) break;
      enqueue(scan);
    }
    lock.lock();
  }
}

ScanDispatcher::ScanDispatcher(std::shared_ptr<DbWriter> writer)
    : m_writer(std::move(writer)), m_running(true) {
  m_schedulerThread = std::thread([this]() { schedulerLoop(); });

  m_collectorThread = std::thread([this]() {
    while (true) {
      {
        std::unique_lock lock(m_collectorQueueMtx);
        m_collectorCv.wait(lock, [&]() { return !m_collectorQueue.empty() || !m_running; });
      }
      if (!m_running && m_collectorQueue.empty()) break;

      int id;
      {
        std::scoped_lock const l(m_collectorQueueMtx);
        id = m_collectorQueue.front();
        m_collectorQueue.pop();
      }
      {
        std::scoped_lock const l(m_scannerMapMtx);
        auto it = m_scannerMap.find(id);
        if (it == m_scannerMap.end()) {
          // Attempted to close the same scanner twice
          continue;
        }
        it->second.scanner->join();
        m_scannerMap.erase(it);
      }
    }
  });
}

bool ScanDispatcher::interrupt(int id) {
  {
    std::scoped_lock const l(m_scannerMapMtx);
    auto element = m_scannerMap.find(id);
    if (element == m_scannerMap.end()) return false;

    element->second.scanner->interrupt();
  }
  return true;
}

void ScanDispatcher::interruptAll() {
  {
    std::scoped_lock const l(m_scannerMapMtx);

    for (auto &[id, element] : m_scannerMap) {
      element.scanner->interrupt();
    }
  }
}

std::vector<std::pair<int, Scan>> ScanDispatcher::scans() {
  {
    std::scoped_lock const l(m_scannerMapMtx);

    return m_scannerMap | std::views::transform([](auto const &it) -> std::pair<int, Scan> {
             return {it.first, it.second.scan};
           }) |
           std::ranges::to<std::vector>();
  }
}

ScanDispatcher::~ScanDispatcher() {
  interruptAll();

  m_running = false;
  m_pendingCv.notify_one();
  m_collectorCv.notify_one();

  m_schedulerThread.join();
  m_collectorThread.join();
}
