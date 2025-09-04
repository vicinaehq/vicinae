#include "abstract-scanner.hpp"
#include <expected>
#include <mutex>
#include <variant>

std::expected<Scan, bool> AbstractScanner::awaitScan() {
  std::unique_lock lock(m_scanLock);

  m_scanCv.wait(lock, [&]() { return !m_queuedScans.empty() || !m_alive; });
  if (!m_alive) { return std::unexpected<bool>(false); }

  Scan front = m_queuedScans.front();

  m_queuedScans.pop_front();
  m_aliveScans.insert(front);

  return front;
}

void AbstractScanner::finishScan(const Scan& scan) {
  std::scoped_lock guard(m_scanLock);
  m_aliveScans.erase(scan);
}

void AbstractScanner::enqueue(const Scan &scan) {
  std::scoped_lock guard(m_scanLock);
  m_queuedScans.push_back(scan);

  m_scanCv.notify_one();
}

void AbstractScanner::run() { m_alive = true; }

void AbstractScanner::stop(bool regurgitate) {
  m_alive = false;
  m_scanCv.notify_one();

  if (regurgitate) {
    for (const auto& scan: m_aliveScans) {
      m_queuedScans.push_front(scan);
    }
  }

  m_aliveScans.clear();
}
