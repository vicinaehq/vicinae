#pragma once
#include <filesystem>
#include <functional>
#include <vector>

class GitIgnoreReader {
  std::vector<std::string> m_patterns;
  std::filesystem::path m_path;

public:
  bool matches(const std::filesystem::path &path) const;

  const std::vector<std::string> &patterns() const;
  GitIgnoreReader(const std::filesystem::path &path);
};

class FileSystemWalker {
  std::vector<std::string> m_ignoreFiles = {".gitignore"};
  bool m_recursive = true;
  bool m_ignoreHiddenFiles = false;
  std::optional<size_t> m_maxDepth;

  bool isIgnored(const std::filesystem::path &path) const;

public:
  using WalkCallback = std::function<void(const std::filesystem::directory_entry &path)>;

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

  void setRecursive(bool value);

  void walk(const std::filesystem::path &path, const WalkCallback &fn);
};
