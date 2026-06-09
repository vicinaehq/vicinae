#pragma once
#include "file-indexer/scan-dispatcher.hpp"
#include "watcher/watcher.hpp"
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

/**
 * We watch non-hidden top level home directories to instantly detect changes made to them
 * We also update those directories at a fixed interval following simple heuristics.
 */
class HomeDirectoryWatcher {
  ScanDispatcher &m_dispatcher;
  std::unique_ptr<wtr::watch> m_watch;
  std::thread m_timerThread;

  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::atomic<bool> m_alive = true;
  std::atomic<bool> m_allowsBackgroundUpdates = true;

  static constexpr size_t BACKGROUND_UPDATE_DEPTH = 5;

  void handleEvent(const wtr::event &ev);
  void timerLoop();
  std::vector<std::filesystem::path> getImportantDirectories();

public:
  HomeDirectoryWatcher(ScanDispatcher &dispatcher);
  ~HomeDirectoryWatcher();
};
