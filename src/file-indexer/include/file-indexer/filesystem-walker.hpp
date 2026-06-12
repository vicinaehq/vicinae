#pragma once
#include "file-indexer/entry-filter.hpp"
#include "file-indexer/io-pacer.hpp"
#include <optional>
#include <filesystem>
#include <functional>
#include <vector>
#include <atomic>

class FileSystemWalker {
public:
  using WalkCallback = std::function<void(const std::filesystem::directory_entry &path)>;
  using DirectoryFilter = std::function<bool(const std::filesystem::directory_entry &dir)>;

  // See `EntryFilter::setIgnoreFiles`
  void setIgnoreFiles(const std::vector<std::string> &files);
  void setMaxDepth(std::optional<size_t> maxDepth);

  /**
   * Do not walk into files and directories that have a leading '.'.
   */
  void setIgnoreHiddenPaths(bool value);
  void setVerbose(bool value = true);
  void setRecursive(bool value);

  void setExcludedPaths(const std::vector<std::filesystem::path> &paths);
  void walk(const std::filesystem::path &path, const WalkCallback &fn);
  void stop();

private:
  EntryFilter m_filter;
  file_indexer::IoPacer m_pacer;
  bool m_recursive = true;
  std::optional<size_t> m_maxDepth;
  bool m_verbose = false;
  std::atomic<bool> m_alive = true;
};
