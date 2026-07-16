#include "file-indexer/scan-dispatcher.hpp"
#include "file-indexer/background-thread.hpp"
#include "file-indexer/indexer-scanner.hpp"
#include "file-indexer/incremental-scanner.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/scan.hpp"
#include <algorithm>
#include <chrono>
#include <format>
#include <memory>
#include <mutex>
#include <ranges>
#include <utility>

namespace {

std::string_view scanTypeLabel(ScanType type) { return type == ScanType::Full ? "full" : "incremental"; }

std::string_view scanModeLabel(const Scan &scan) {
  if (const auto *incremental = std::get_if<IncrementalScan>(&scan.data)) {
    return incremental->mode == ScanMode::Exhaustive ? "exhaustive" : "pruned";
  }

  return "exhaustive";
}

flog::LogStream scanLog(const Scan &scan) {
  return scan.type() == ScanType::Full ? flog::info() : flog::debug();
}

} // namespace

int ScanDispatcher::enqueue(const Scan &scan) {
  {
    std::scoped_lock const l(m_workMtx);

    bool const duplicate =
        std::ranges::any_of(m_running | std::views::values,
                            [&](const Running &running) { return running.scan.path == scan.path; }) ||
        std::ranges::any_of(m_ready, [&](const auto &ready) { return ready.second.path == scan.path; });

    if (duplicate) {
      scanLog(scan) << std::format("Skipping {} scan for {} (already running)", scanTypeLabel(scan.type()),
                                   scan.path.c_str());
      return -1;
    }

    int const scanId = m_nextScanId++;

    scanLog(scan) << std::format("Enqueuing {} scan for {} ({})", scanTypeLabel(scan.type()),
                                 scan.path.c_str(), scanModeLabel(scan));
    m_ready.emplace_back(scanId, scan);
    m_workCv.notify_one();
    return scanId;
  }
}

std::unique_ptr<AbstractScanner> ScanDispatcher::makeScanner(int scanId, const Scan &scan,
                                                             FileIndexerDatabase &readDb) {
  auto handler = [this, scanId, path = scan.path, type = scan.type(),
                  notify = scan.notify](ScanStatus status, size_t processedCount) {
    if (notify && m_eventCallback) {
      m_eventCallback({.scanId = scanId,
                       .type = type,
                       .status = status,
                       .entrypoint = path,
                       .processedFileCount = processedCount});
    }
  };

  switch (scan.type()) {
  case ScanType::Full:
    return std::make_unique<IndexerScanner>(m_writer, scan, std::move(handler));
  case ScanType::Incremental:
    return std::make_unique<IncrementalScanner>(m_writer, scan, readDb, std::move(handler));
  }

  return nullptr;
}

void ScanDispatcher::workerLoop() {
  file_indexer::setBackgroundThreadPriority();
  FileIndexerDatabase readDb;

  std::unique_lock lock(m_workMtx);

  while (true) {
    m_workCv.wait(lock, [&] { return !m_ready.empty() || !m_alive; });
    if (!m_alive) break;

    auto [scanId, scan] = std::move(m_ready.front());
    m_ready.pop_front();

    auto it =
        m_running.emplace(scanId, Running{std::move(scan), nullptr, std::chrono::steady_clock::now()}).first;
    it->second.scanner = makeScanner(scanId, it->second.scan, readDb);
    auto *scanner = it->second.scanner.get();

    lock.unlock();
    scanner->run();
    lock.lock();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() -
                                                                          it->second.startedAt)
                        .count();

    scanLog(it->second.scan) << std::format("Done scanning {} in {}ms ({}, {})", it->second.scan.path.c_str(),
                                            duration, scanTypeLabel(it->second.scan.type()),
                                            scanModeLabel(it->second.scan));

    m_running.erase(it);
    m_idleCv.notify_all();
  }
}

void ScanDispatcher::enqueueDebounced(const Scan &scan) {
  auto const now = std::chrono::steady_clock::now();
  {
    std::scoped_lock const l(m_pendingMtx);
    mergePendingLocked(scan, now);
  }
  m_pendingCv.notify_one();
}

void ScanDispatcher::mergePendingLocked(const Scan &scan, std::chrono::steady_clock::time_point now) {
  auto it = std::ranges::find_if(m_pending, [&](const Pending &pending) {
    return pending.scan.path == scan.path && pending.scan.type() == scan.type();
  });

  if (it == m_pending.end()) {
    m_pending.emplace_back(scan, now + DEBOUNCE_QUIET, now);
    return;
  }

  it->deadline = std::min(now + DEBOUNCE_QUIET, it->firstSeen + DEBOUNCE_MAX_DELAY);
}

void ScanDispatcher::schedulerLoop() {
  std::unique_lock lock(m_pendingMtx);

  while (m_alive) {
    if (m_pending.empty()) {
      m_pendingCv.wait(lock, [&]() { return !m_pending.empty() || !m_alive; });
      continue;
    }

    auto const next = std::ranges::min_element(m_pending, {}, &Pending::deadline)->deadline;
    if (m_pendingCv.wait_until(lock, next, [&]() { return !m_alive.load(); })) break;

    auto const now = std::chrono::steady_clock::now();
    std::vector<Scan> due;

    due.reserve(m_pending.size());
    std::erase_if(m_pending, [&](const Pending &pending) {
      if (pending.deadline > now) return false;
      due.emplace_back(pending.scan);
      return true;
    });

    lock.unlock();
    std::vector<Scan> deferred;
    deferred.reserve(due.size());

    for (auto &scan : due) {
      if (!m_alive) break;
      if (enqueue(scan) < 0) { deferred.emplace_back(std::move(scan)); }
    }

    lock.lock();
    auto const rearmTime = std::chrono::steady_clock::now();
    for (const auto &scan : deferred) {
      mergePendingLocked(scan, rearmTime);
    }
  }
}

ScanDispatcher::ScanDispatcher(std::shared_ptr<DbWriter> writer)
    : m_writer(std::move(writer)), m_alive(true) {
  m_schedulerThread = std::thread([this]() { schedulerLoop(); });

  m_workers.reserve(WORKER_COUNT);
  for (size_t i = 0; i < WORKER_COUNT; ++i) {
    m_workers.emplace_back([this]() { workerLoop(); });
  }
}

bool ScanDispatcher::interrupt(int id) {
  std::scoped_lock const l(m_workMtx);

  if (auto it = m_running.find(id); it != m_running.end()) {
    it->second.scanner->interrupt();
    return true;
  }

  if (auto it = std::ranges::find(m_ready, id, &std::pair<int, Scan>::first); it != m_ready.end()) {
    m_ready.erase(it);
    m_idleCv.notify_all();
    return true;
  }

  return false;
}

void ScanDispatcher::interruptAll() {
  std::scoped_lock const l(m_workMtx);

  m_ready.clear();
  for (auto &[id, running] : m_running) {
    running.scanner->interrupt();
  }
  m_idleCv.notify_all();
}

void ScanDispatcher::clearPending() {
  std::scoped_lock const l(m_pendingMtx);
  m_pending.clear();
}

void ScanDispatcher::waitUntilIdle() {
  std::unique_lock lock(m_workMtx);
  m_idleCv.wait(lock, [this] { return m_running.empty() && m_ready.empty(); });
}

std::vector<std::pair<int, Scan>> ScanDispatcher::scans() {
  std::scoped_lock const l(m_workMtx);

  std::vector<std::pair<int, Scan>> result;
  result.reserve(m_running.size() + m_ready.size());

  for (auto const &[id, running] : m_running) {
    result.emplace_back(id, running.scan);
  }
  for (auto const &[id, scan] : m_ready) {
    result.emplace_back(id, scan);
  }

  return result;
}

ScanDispatcher::~ScanDispatcher() {
  clearPending();
  interruptAll();
  waitUntilIdle();

  m_alive = false;
  m_pendingCv.notify_all();
  m_workCv.notify_all();

  m_schedulerThread.join();
  for (auto &worker : m_workers) {
    worker.join();
  }
}
