#pragma once
#include "file-indexer/util.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/entry-filter.hpp"
#include "file-indexer/io-pacer.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <thread>
#include <unordered_set>
#include <vector>

class IncrementalScanner : public AbstractScanner, file_indexer::NonCopyable {
  using EntryCallback = std::function<void(const std::filesystem::directory_entry &entry, bool isNew)>;

  std::unique_ptr<FileIndexerDatabase> m_read_db;
  std::thread m_scanThread;
  EntryFilter m_filter;
  file_indexer::IoPacer m_pacer;
  std::unordered_set<std::filesystem::path> m_currentEntries;

  std::vector<std::filesystem::path>
  getScannableDirectories(const std::filesystem::path &path, std::optional<size_t> maxDepth,
                          const std::vector<std::filesystem::path> &excludedPaths);

  // diffs the direct contents of `path` against the index; `isNew` flags entries
  // that were previously unindexed
  void processDirectory(const std::filesystem::path &path, const EntryCallback &onEntry = {});
  bool shouldProcessEntry(const std::filesystem::directory_entry &entry, int64_t cutOffSeconds) const;

  void scan(const std::filesystem::path &path, const IncrementalScan &scan);
  void exhaustiveScan(const std::filesystem::path &path, const IncrementalScan &scan);
  void prunedScan(const std::filesystem::path &path, const IncrementalScan &scan);

public:
  IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, StatusCallback callback);
  ~IncrementalScanner() = default;

  void interrupt() override;
  void join() override;
};
