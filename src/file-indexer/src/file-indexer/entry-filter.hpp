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

class EntryFilter {
public:
  void setExcludedPaths(std::vector<std::filesystem::path> paths);
  void setExcludedFilenames(std::vector<std::string> filenames);

  void setIgnoreFiles(std::vector<std::string> files);

  void setIgnoreHiddenPaths(bool value);

  bool shouldVisit(const std::filesystem::directory_entry &entry) const;

  static bool isCacheDirTag(const std::filesystem::path &entryPath);

private:
  std::vector<std::filesystem::path> m_excludedPaths;
  std::vector<std::string> m_excludedFilenames;
  std::vector<std::string> m_ignoreFiles;
  bool m_ignoreHiddenPaths = false;

  bool isIgnored(const std::filesystem::path &path) const;
  bool isExcludedPath(const std::filesystem::path &path) const;
};
