#pragma once
#include <chrono>
#include <expected>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "services/files-service/abstract-file-indexer.hpp"

namespace vicinae::fff {

/**
 * Thin RAII wrapper over the fff C API (https://github.com/dmtrKovalenko/fff,
 * `crates/fff-c`). Scoped to what vicinae needs for path search: create an
 * instance, wait for the initial scan, run fuzzy searches, optionally swap
 * the root directory. We intentionally don't expose grep / content indexing.
 *
 * Thread safety: the underlying fff instance is internally synchronized, so
 * `search()` and `progress()` may be called from any thread for as long as
 * the object lives. Destruction must be race-free with any in-flight
 * callers; in practice the owning `FffFileIndexer` pins the shared_ptr for
 * the duration of each query.
 */
class FffInstance {
public:
  struct Config {
    std::filesystem::path basePath;
    std::filesystem::path frecencyDbPath;
    std::filesystem::path historyDbPath;
    bool enableMmapCache = false;
    bool enableContentIndexing = false;
    bool watch = false;
    bool aiMode = false;
  };

  struct ScanProgress {
    std::uint64_t scannedFilesCount = 0;
    bool isScanning = false;
    bool isWatcherReady = false;
    bool isWarmupComplete = false;
  };

  struct SearchOptions {
    int pageIndex = 0;
    int pageSize = 100;
    int maxThreads = 0;
  };

  static std::expected<std::unique_ptr<FffInstance>, std::string> create(const Config &config);

  FffInstance(const FffInstance &) = delete;
  FffInstance &operator=(const FffInstance &) = delete;
  FffInstance(FffInstance &&other) noexcept;
  FffInstance &operator=(FffInstance &&other) noexcept;
  ~FffInstance();

  /**
   * Block the calling thread until the background scan completes (or the
   * timeout elapses). Safe to call from a worker thread. Returns true if
   * the scan completed, false on timeout / error.
   */
  bool waitForScan(std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) const;

  ScanProgress progress() const;

  std::vector<IndexerFileResult> search(std::string_view query, const SearchOptions &options) const;

  /**
   * Kick off a rescan of the configured base_path. Results of in-flight
   * searches are undefined; callers should treat this as a readiness reset.
   */
  bool rescan();

  /**
   * Swap the indexed base path. The instance enters a not-ready state again;
   * call `waitForScan` to block until the new warmup finishes.
   */
  bool restartIndex(const std::filesystem::path &newBase);

  const std::filesystem::path &basePath() const { return m_basePath; }

private:
  FffInstance(void *handle, std::filesystem::path basePath);

  void *m_handle = nullptr;
  std::filesystem::path m_basePath;
};

} // namespace vicinae::fff
