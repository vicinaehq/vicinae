#include "file-indexer/file-system-watcher.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/scan.hpp"
#include <chrono>

namespace fs = std::filesystem;

void FileSystemWatcher::handleEvent(const ImportantDirectoryWatcher::Event &ev) {
  using Kind = ImportantDirectoryWatcher::Event::Kind;

  switch (ev.kind) {
  case Kind::DirectoryChanged:
    m_dispatcher.enqueueDebounced({.path = ev.dir, .data = incrementalScan(ScanMode::Pruned)});
    break;
  case Kind::Degraded:
    flog::warn() << "Directory watcher degraded, rescanning all watched roots";
    for (const auto &dir : m_watcher->rootDirectories()) {
      m_dispatcher.enqueueDebounced({.path = dir, .data = incrementalScan(ScanMode::Pruned)});
    }
    break;
  }
}

IncrementalScan FileSystemWatcher::incrementalScan(ScanMode mode) const {
  return {.mode = mode, .excludedFilenames = m_excludedFilenames, .excludedPaths = m_excludedPaths};
}

void FileSystemWatcher::timerLoop() {
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
      for (const auto &entrypoint : m_entrypoints) {
        if (fs::is_directory(entrypoint, ec)) {
          auto scan = incrementalScan();
          scan.maxDepth = BACKGROUND_UPDATE_DEPTH;
          m_dispatcher.enqueue({.path = entrypoint, .data = std::move(scan)});
        }
      }

      refreshDynamicWatches();
    }
  }
}

void FileSystemWatcher::refreshDynamicWatches() {
  m_watcher->setDynamicDirectories(m_readDb->listRecentDirectories(DYNAMIC_WATCH_COUNT));
}

FileSystemWatcher::FileSystemWatcher(ScanDispatcher &dispatcher, std::vector<fs::path> entrypoints,
                                     std::vector<fs::path> excludedPaths,
                                     std::vector<std::string> excludedFilenames)
    : m_dispatcher(dispatcher), m_entrypoints(std::move(entrypoints)),
      m_excludedPaths(std::move(excludedPaths)), m_excludedFilenames(std::move(excludedFilenames)) {
  m_readDb = std::make_unique<FileIndexerDatabase>();
  m_watcher = ImportantDirectoryWatcher::create(
      [this](const ImportantDirectoryWatcher::Event &ev) { handleEvent(ev); });

  m_timerThread = std::thread([this] { timerLoop(); });
}

FileSystemWatcher::~FileSystemWatcher() {
  m_alive = false;
  m_cv.notify_all();
  if (m_timerThread.joinable()) m_timerThread.join();
}
