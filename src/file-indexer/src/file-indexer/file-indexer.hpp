#pragma once
#include "file-indexer/file-indexer-query-engine.hpp"
#include "file-indexer/home-directory-watcher.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/scan-dispatcher.hpp"
#include <chrono>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/**
 * Generic file indexer that should be usable in most linux environments.
 * SQLite FTS5 is used as a backend, making it technically possible to index the entire
 * filesystem if necessary.
 * Queries usually remain very fast (<100ms), although not fast enough to perform them in the UI thread
 * without introducing slowdowns.
 */
class FileIndexer {
public:
  std::shared_ptr<DbWriter> m_writer;
  std::vector<std::filesystem::path> m_entrypoints;
  std::vector<std::filesystem::path> m_excludedPaths;
  std::vector<std::string> m_excludedFilenames;
  FileIndexerQueryEngine m_queryEngine;
  FileIndexerDatabase m_db;

  ScanDispatcher m_dispatcher;
  ScanDispatcher::EventCallback m_scanEventCallback;

  std::mutex m_homeWatcherMtx;
  std::unique_ptr<HomeDirectoryWatcher> m_homeWatcher;

  std::mutex m_pendingFullScanRootsMtx;
  std::vector<std::filesystem::path> m_pendingFullScanRoots;

  void startHomeWatcher();
  void stopHomeWatcher();
  void markFullScanRootsPending(const std::vector<std::filesystem::path> &roots);
  void markFullScanSucceeded(const std::filesystem::path &root);
  void prunePendingFullScans(const std::vector<std::filesystem::path> &roots,
                             const std::vector<std::filesystem::path> &exclusions);
  std::vector<std::filesystem::path>
  pendingFullScanRootsFor(const std::vector<std::filesystem::path> &roots,
                          const std::vector<std::filesystem::path> &exclusions);

  // rebuilds scan the entire index: don't redo it for every watcher-triggered scan
  static constexpr std::chrono::minutes VOCAB_REBUILD_MIN_INTERVAL{10};
  std::mutex m_vocabRebuildMtx;
  std::chrono::steady_clock::time_point m_lastVocabRebuild{};

  bool shouldRebuildVocabulary();

public:
  void startFullScan();
  void startSingleScan(const std::filesystem::path &entrypoint, ScanType type,
                       const std::vector<std::string> &excludedFilenames = {});
  void markScanAsInterrupted(std::optional<FileIndexerDatabase::ScanRecord> scan);
  void setScanEventCallback(ScanDispatcher::EventCallback callback) {
    m_scanEventCallback = std::move(callback);
  }
  void rebuildIndex();
  void setConfig(std::vector<std::filesystem::path> paths, std::vector<std::filesystem::path> excludedPaths);
  void applyConfig(std::vector<std::filesystem::path> paths,
                   std::vector<std::filesystem::path> excludedPaths);
  std::vector<IndexerFileResult> query(std::string_view view, int limit,
                                       const FileIndexerQueryEngine::QueryOptions &options = {});
  void start();

  FileIndexer();
};
