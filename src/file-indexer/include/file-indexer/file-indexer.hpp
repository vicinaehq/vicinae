#pragma once
#include "file-indexer/file-indexer-query-engine.hpp"
#include "file-indexer/home-directory-watcher.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/scan-dispatcher.hpp"
#include <filesystem>
#include <memory>
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
  std::vector<std::filesystem::path> m_watcherPaths;
  std::vector<std::string> m_excludedFilenames;
  FileIndexerQueryEngine m_queryEngine;
  FileIndexerDatabase m_db;

  ScanDispatcher m_dispatcher;

  std::unique_ptr<HomeDirectoryWatcher> m_homeWatcher;

public:
  void startFullScan();
  void startSingleScan(const std::filesystem::path &entrypoint, ScanType type,
                       const std::vector<std::string> &excludedFilenames = {});
  void markScanAsInterrupted(std::optional<FileIndexerDatabase::ScanRecord> scan);
  void rebuildIndex();
  void setConfig(std::vector<std::filesystem::path> paths, std::vector<std::filesystem::path> excludedPaths,
                 std::vector<std::filesystem::path> watcherPaths);
  std::vector<IndexerFileResult> query(std::string_view view, const Pagination &pagination = {});
  void start();

  FileIndexer();
};
