#include "file-indexer/home-directory-watcher.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/scan.hpp"
#include <chrono>

namespace fs = std::filesystem;

void HomeDirectoryWatcher::handleEvent(const wtr::event &ev) {
  if (ev.path_type == wtr::event::path_type::watcher) return;

  // only structural changes affect a directory's listing
  switch (ev.effect_type) {
  case wtr::event::effect_type::create:
  case wtr::event::effect_type::destroy:
  case wtr::event::effect_type::rename:
    break;
  default:
    return;
  }

  fs::path const parent = fs::path(ev.path_name).parent_path();
  if (parent.empty()) return;

  m_dispatcher.enqueue({.type = ScanType::Incremental, .path = parent, .maxDepth = 1});
}

std::vector<std::filesystem::path> HomeDirectoryWatcher::getImportantDirectories() {
  return {file_indexer::documentsFolder(), file_indexer::downloadsFolder()};
}

void HomeDirectoryWatcher::timerLoop() {
  using namespace std::chrono_literals;
  using namespace std::chrono;

  auto lastHourly = steady_clock::now();
  auto lastImportant = steady_clock::now();

  while (m_alive) {
    {
      std::unique_lock lock(m_mtx);
      m_cv.wait_for(lock, 1min, [&] { return !m_alive.load(); });
    }
    if (!m_alive) break;
    if (!m_allowsBackgroundUpdates) continue;

    auto const now = steady_clock::now();
    std::error_code ec;

    if (now - lastImportant >= 10min) {
      lastImportant = now;
      for (const auto &dir : getImportantDirectories()) {
        if (fs::is_directory(dir, ec)) {
          m_dispatcher.enqueue(
              {.type = ScanType::Incremental, .path = dir, .maxDepth = BACKGROUND_UPDATE_DEPTH});
        }
      }
    }

    if (now - lastHourly >= 1h) {
      lastHourly = now;
      for (const auto &entry : fs::directory_iterator(file_indexer::homeDir(), ec)) {
        if (!entry.is_directory(ec)) continue;
        if (file_indexer::isHiddenPath(entry.path())) continue;
        m_dispatcher.enqueue(
            {.type = ScanType::Incremental, .path = entry.path(), .maxDepth = BACKGROUND_UPDATE_DEPTH});
      }
    }
  }
}

HomeDirectoryWatcher::HomeDirectoryWatcher(ScanDispatcher &dispatcher) : m_dispatcher(dispatcher) {
  m_watch = std::make_unique<wtr::watch>(file_indexer::homeDir(),
                                         [this](const wtr::event &ev) { handleEvent(ev); });
  m_timerThread = std::thread([this] { timerLoop(); });
}

HomeDirectoryWatcher::~HomeDirectoryWatcher() {
  m_alive = false;
  m_cv.notify_all();
  if (m_timerThread.joinable()) m_timerThread.join();
  if (m_watch) m_watch->close();
}
