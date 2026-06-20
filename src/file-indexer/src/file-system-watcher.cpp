#include "file-indexer/file-system-watcher.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/util.hpp"
#include <chrono>

namespace fs = std::filesystem;

void FileSystemWatcher::handleEvent(const ImportantDirectoryWatcher::Event &ev) {
  using Kind = ImportantDirectoryWatcher::Event::Kind;

  switch (ev.kind) {
  case Kind::DirectoryChanged:
    if (shouldScanPath(ev.dir)) {
      m_dispatcher.enqueueDebounced(
          {.path = file_indexer::normalizePath(ev.dir), .data = incrementalScan(ScanMode::Pruned)});
    }
    break;
  case Kind::Degraded:
    flog::warn() << "Directory watcher degraded, rescanning configured roots";
    for (const auto &dir : m_entrypoints) {
      if (shouldScanPath(dir)) {
        m_dispatcher.enqueueDebounced({.path = dir, .data = incrementalScan(ScanMode::Pruned)});
      }
    }
    break;
  }
}

IncrementalScan FileSystemWatcher::incrementalScan(ScanMode mode) const {
  return {.mode = mode, .excludedFilenames = m_excludedFilenames, .excludedPaths = m_excludedPaths};
}

bool FileSystemWatcher::shouldScanPath(const fs::path &path) const {
  auto const normalized = file_indexer::normalizePath(path);

  auto containsPath = [&](const std::vector<fs::path> &roots) {
    return file_indexer::isCoveredByAny(normalized, roots);
  };

  return containsPath(m_entrypoints) && !containsPath(m_excludedPaths);
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
  auto dirs = m_readDb->listRecentDirectories(DYNAMIC_WATCH_COUNT);
  std::erase_if(dirs, [this](const fs::path &dir) { return !shouldScanPath(dir); });
  m_watcher->setDynamicDirectories(dirs);
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
