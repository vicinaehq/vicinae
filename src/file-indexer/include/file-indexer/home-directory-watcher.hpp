#pragma once
#include "file-indexer/important-dir-watcher.hpp"
#include "file-indexer/scan-dispatcher.hpp"
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

/**
 * Watches important home directories to quickly detect changes made to them.
 * We also update those directories at a fixed interval following simple heuristics.
 */
class HomeDirectoryWatcher {
  ScanDispatcher &m_dispatcher;
  std::unique_ptr<ImportantDirectoryWatcher> m_watcher;
  std::thread m_timerThread;

  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::atomic<bool> m_alive = true;
  std::atomic<bool> m_allowsBackgroundUpdates = true;

  static constexpr size_t BACKGROUND_UPDATE_DEPTH = 5;

  void handleEvent(const ImportantDirectoryWatcher::Event &ev);
  void timerLoop();
  std::vector<std::filesystem::path> getImportantDirectories();

public:
  HomeDirectoryWatcher(ScanDispatcher &dispatcher);
  ~HomeDirectoryWatcher();
};
