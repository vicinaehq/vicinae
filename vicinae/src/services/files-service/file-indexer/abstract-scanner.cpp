#include "abstract-scanner.hpp"
#include <expected>
#include <variant>

std::expected<Scan, bool> AbstractScanner::awaitScan() {
  if (!m_alive)
    return std::unexpected<bool>(false);

  std::unique_lock lock(m_scanLock);
  m_scanCv.wait(lock, [&]() {return !m_scans.empty() || !m_alive;});
  if (!m_alive) {
    return std::unexpected<bool>(false);
  }
  Scan front = m_scans.front();
  m_scans.pop();
  return front;
}

void AbstractScanner::enqueue(const Scan& scan) {
  m_scanLock.lock();
  m_scans.push(scan);
  m_scanLock.unlock();

  m_scanCv.notify_one();
}

void AbstractScanner::run() {
  m_alive = true;
}

void AbstractScanner::stop() {
  m_alive = false;
  m_scanCv.notify_one();
}
