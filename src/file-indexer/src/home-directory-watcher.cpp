#include "file-indexer/home-directory-watcher.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/scan.hpp"
#include <chrono>

namespace fs = std::filesystem;

void HomeDirectoryWatcher::handleEvent(const ImportantDirectoryWatcher::Event &ev) {
  using Kind = ImportantDirectoryWatcher::Event::Kind;

  switch (ev.kind) {
  case Kind::DirectoryChanged:
    m_dispatcher.enqueueDebounced({.path = ev.dir, .data = IncrementalScan{.mode = ScanMode::Pruned}});
    break;
  case Kind::Degraded:
    flog::warn() << "Directory watcher degraded, rescanning all watched roots";
    for (const auto &dir : m_watcher->rootDirectories()) {
      m_dispatcher.enqueueDebounced({.path = dir, .data = IncrementalScan{.mode = ScanMode::Pruned}});
    }
    break;
  }
}

std::vector<std::filesystem::path> HomeDirectoryWatcher::getImportantDirectories() {
  // the home root's children are each swept individually, and deeper
  auto dirs = m_watcher->rootDirectories();
  std::erase(dirs, file_indexer::homeDir());
  return dirs;
}

void HomeDirectoryWatcher::timerLoop() {
  using namespace std::chrono_literals;
  using namespace std::chrono;

  auto lastSweep = steady_clock::now();

  refreshDynamicWatches();

  while (m_alive) {
    {
      std::unique_lock lock(m_mtx);
      m_cv.wait_for(lock, 1min, [&] { return !m_alive.load(); });
    }
    if (!m_alive) break;
    if (!m_allowsBackgroundUpdates) continue;

    auto const now = steady_clock::now();
    std::error_code ec;

    if (now - lastSweep >= BACKGROUND_UPDATE_INTERVAL) {
      lastSweep = now;
      for (const auto &dir : getImportantDirectories()) {
        if (fs::is_directory(dir, ec)) {
          m_dispatcher.enqueue({.path = dir, .data = IncrementalScan{.maxDepth = BACKGROUND_UPDATE_DEPTH}});
        }
      }

      refreshDynamicWatches();
    }
  }
}

void HomeDirectoryWatcher::refreshDynamicWatches() {
  m_watcher->setDynamicDirectories(m_readDb->listRecentDirectories(DYNAMIC_WATCH_COUNT));
}

HomeDirectoryWatcher::HomeDirectoryWatcher(ScanDispatcher &dispatcher) : m_dispatcher(dispatcher) {
  m_readDb = std::make_unique<FileIndexerDatabase>();
  m_watcher = ImportantDirectoryWatcher::create(
      [this](const ImportantDirectoryWatcher::Event &ev) { handleEvent(ev); });

  m_timerThread = std::thread([this] { timerLoop(); });
}

HomeDirectoryWatcher::~HomeDirectoryWatcher() {
  m_alive = false;
  m_cv.notify_all();
  if (m_timerThread.joinable()) m_timerThread.join();
}
