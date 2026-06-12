#pragma once
#include <filesystem>
#include <string>
#include <vector>

class GitIgnoreReader {
  std::vector<std::string> m_patterns;
  std::filesystem::path m_path;

public:
  bool matches(const std::filesystem::path &path) const;

  const std::vector<std::string> &patterns() const;
  GitIgnoreReader(const std::filesystem::path &path);
};

// Single exclusion policy for every component that traverses the filesystem.
class EntryFilter {
public:
  void setExcludedPaths(std::vector<std::filesystem::path> paths);
  void setExcludedFilenames(std::vector<std::string> filenames);

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
  void setIgnoreFiles(std::vector<std::string> files);

  /**
   * Do not visit files and directories that have a leading '.'.
   */
  void setIgnoreHiddenPaths(bool value);

  bool shouldVisit(const std::filesystem::directory_entry &entry) const;

  /*
   * Cache Directory Tagging Specification: directories containing a valid CACHEDIR.TAG
   * file hold machine-generated cache data and should not be indexed.
   * Notably written by cargo in every target/ directory.
   */
  static bool isCacheDirTag(const std::filesystem::path &entryPath);

private:
  std::vector<std::filesystem::path> m_excludedPaths;
  std::vector<std::string> m_excludedFilenames;
  std::vector<std::string> m_ignoreFiles;
  bool m_ignoreHiddenPaths = false;

  bool isIgnored(const std::filesystem::path &path) const;
  bool isExcludedPath(const std::filesystem::path &path) const;
};
