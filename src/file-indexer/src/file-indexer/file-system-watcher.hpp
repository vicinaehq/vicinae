#pragma once
#include "file-indexer/important-dir-watcher.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/scan-dispatcher.hpp"
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

class FileSystemWatcher {
  ScanDispatcher &m_dispatcher;
  std::unique_ptr<ImportantDirectoryWatcher> m_watcher;
  std::unique_ptr<FileIndexerDatabase> m_readDb;
  std::thread m_timerThread;
  std::vector<std::filesystem::path> m_entrypoints;
  std::vector<std::filesystem::path> m_excludedPaths;
  std::vector<std::string> m_excludedFilenames;

  std::mutex m_mtx;
  std::condition_variable m_cv;
  std::atomic<bool> m_alive = true;
  std::atomic<bool> m_allowsBackgroundUpdates = true;

  static constexpr size_t BACKGROUND_UPDATE_DEPTH = 5;
  static constexpr std::chrono::minutes BACKGROUND_UPDATE_INTERVAL{1};
  static constexpr int DYNAMIC_WATCH_COUNT = 2048;

  void handleEvent(const ImportantDirectoryWatcher::Event &ev);
  IncrementalScan incrementalScan(ScanMode mode = ScanMode::Exhaustive) const;
  bool shouldScanPath(const std::filesystem::path &path) const;
  void timerLoop();
  void refreshDynamicWatches();

public:
  FileSystemWatcher(ScanDispatcher &dispatcher, std::vector<std::filesystem::path> entrypoints,
                    std::vector<std::filesystem::path> excludedPaths,
                    std::vector<std::string> excludedFilenames);
  ~FileSystemWatcher();
};
