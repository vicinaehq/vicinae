#pragma once
#include <optional>
#include <filesystem>
#include <functional>
#include <vector>
#include <atomic>

class GitIgnoreReader {
  std::vector<std::string> m_patterns;
  std::filesystem::path m_path;

public:
  bool matches(const std::filesystem::path &path) const;

  const std::vector<std::string> &patterns() const;
  GitIgnoreReader(const std::filesystem::path &path);
};

class FileSystemWalker {
public:
  using WalkCallback = std::function<void(const std::filesystem::directory_entry &path)>;
  using DirectoryFilter = std::function<bool(const std::filesystem::directory_entry &dir)>;

  /**
   * Register specific filenames for them to be considered as ignore files.
   *
   * Ignore files are used to skip entire directory hierarchies when they match one
   * of the patterns present in these files.
   *
   * Similary to git, all ignore files located above the path that is being scrutinized are considered.
   * Relative patterns (without leading '/') are considered with respect to the path location.
   *
   * Patterns are interpreted using `fnmatch` which is not exactly conform to the logic git uses
   * internally. For that reason, very complex patterns may not be interpreted correctly.
   *
   * // TODO: consider implementing a gitignore parsing C++ library (I haven't found any).
   *
   * By default, we only honor `.gitignore` files.
   */
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
  // we don't use ignore files by default
  std::vector<std::string> m_ignoreFiles = {};
  bool m_recursive = true;
  bool m_ignoreHiddenFiles = false;
  std::optional<size_t> m_maxDepth;
  bool m_verbose = false;
  std::atomic<bool> m_alive = true;
  std::vector<std::filesystem::path> m_excludedPaths = {};

  bool isIgnored(const std::filesystem::path &path) const;
  bool isExcludedPath(const std::filesystem::path &path) const;
};
