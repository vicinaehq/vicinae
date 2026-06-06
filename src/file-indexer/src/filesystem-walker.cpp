#include "file-indexer/filesystem-walker.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/util.hpp"
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <fnmatch.h>
#include <stack>
#include <string>

namespace fs = std::filesystem;

/*
 * List of absolute paths to never follow during indexing. Mostly used to exclude
 * pseudo filesystems such as /run or /proc.
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::vector<fs::path> EXCLUDED_PATHS = {"/sys",     "/run", "/proc", "/tmp",
                                                     "/var/tmp", "/efi", "/dev"};

/**
 * Filenames that can always be ignored. If any file names are to be added here, it's important to make sure
 * they have a specific enough name so that it doesn't generate false positives and prevent indexing actually
 * meaningful content. If you are not sure, it's better to not add it.
 *
 * The indexer is pretty fast and can index millions of files without issue, so indexing some garbage
 * is easily forgivable.
 */
static const std::vector<std::string_view> EXCLUDED_FILENAMES = {
    ".git",
    ".cache",
    ".clangd",
};

bool GitIgnoreReader::matches(const fs::path &path) const {
  for (const auto &pattern : m_patterns) {
    std::string const filename = file_indexer::getLastPathComponent(path);
    std::string processedPattern = pattern;

    if (pattern.starts_with('/')) {
      // get rid of leading slash to match more gitignore patterns - this is a hack:
      // directory separators are not handled properly.
      processedPattern = pattern.substr(1);
    }

    if (fnmatch(processedPattern.c_str(), filename.c_str(), FNM_PATHNAME | FNM_EXTMATCH) == 0) {
      return true;
    }
  }

  return false;
}

const std::vector<std::string> &GitIgnoreReader::patterns() const { return m_patterns; }

GitIgnoreReader::GitIgnoreReader(const fs::path &path) : m_path(path) {
  std::ifstream ifs(path);
  std::string line;

  while (std::getline(ifs, line)) {
    m_patterns.emplace_back(line);
  }
}

void FileSystemWalker::setIgnoreFiles(const std::vector<std::string> &files) { m_ignoreFiles = files; }

void FileSystemWalker::setExcludedPaths(const std::vector<std::filesystem::path> &paths) {
  m_excludedPaths = paths;
}

void FileSystemWalker::setRecursive(bool value) { m_recursive = value; }

void FileSystemWalker::setMaxDepth(std::optional<size_t> maxDepth) { m_maxDepth = maxDepth; }

void FileSystemWalker::setIgnoreHiddenPaths(bool value) { m_ignoreHiddenFiles = value; }

void FileSystemWalker::setVerbose(bool value) { m_verbose = value; }

bool FileSystemWalker::isIgnored(const std::filesystem::path &path) const {
  if (m_ignoreFiles.empty()) return false;

  fs::path p = path.parent_path();
  std::error_code ec;

  while (p != p.root_directory()) {
    for (const auto &name : m_ignoreFiles) {
      fs::path const ignorePath = p / name;

      if (!fs::is_regular_file(ignorePath, ec)) continue;
      if (GitIgnoreReader(ignorePath).matches(path)) { return true; }
    }

    p = p.parent_path();
  }

  return false;
}

bool FileSystemWalker::isExcludedPath(const std::filesystem::path &path) const {
  return std::ranges::find(m_excludedPaths, path) != m_excludedPaths.end();
}

void FileSystemWalker::walk(const fs::path &root, const WalkCallback &callback) {
  using namespace std::chrono;

  auto start = high_resolution_clock::now();
  std::stack<fs::path> dirStack;
  size_t const rootDepth = std::distance(root.begin(), root.end());
  size_t dirCount = 0;
  size_t fileCount = 0;
  std::error_code ec;

  if (!fs::is_directory(root, ec)) {
    flog::warn() << "FileSystemWalker needs to be passed a readable directory as its root entrypoint"
                 << (ec ? ec.message().c_str() : "");
    return;
  }

  dirStack.push(root);

  while (!dirStack.empty()) {
    if (!m_alive) break;

    auto path = dirStack.top();

    dirStack.pop();

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (ec) {
        flog::warn() << "walk error" << ec.message().c_str();
        continue;
      }
      if (entry.is_symlink(ec)) { continue; }

      const auto &path = entry.path();

      if (m_ignoreHiddenFiles && file_indexer::isHiddenPath(path)) {
        if (m_verbose) { flog::info() << "FileSystemWalker: ignoring hidden path" << path.c_str(); }
        continue;
      }

      if (std::ranges::find(EXCLUDED_PATHS, path) != EXCLUDED_PATHS.end()) { continue; }
      if (std::ranges::find(EXCLUDED_FILENAMES, path.filename()) != EXCLUDED_FILENAMES.end()) { continue; }
      if (isIgnored(path)) {
        if (m_verbose) { flog::info() << "Indexing: ignoring git-ignored path" << path.c_str(); }
        continue;
      }

      if (isExcludedPath(path)) {
        if (m_verbose) { flog::info() << "FileSystemWalker: excluding path" << path.c_str(); }
        continue;
      }

      bool const isDir = entry.is_directory(ec);

      if (isDir) {
        ++dirCount;
      } else {
        ++fileCount;
      }

      if (m_recursive && isDir) {
        size_t const depth = std::distance(path.begin(), path.end()) - rootDepth;
        bool const shouldDescend = !m_maxDepth || depth <= *m_maxDepth;
        if (shouldDescend) dirStack.push(entry);
      }

      callback(entry);
    }
  }

  double const duration = duration_cast<seconds>(high_resolution_clock::now() - start).count();

  flog::info() << std::format("Done walking file tree at {}. Processed {} directories and {} files in {} "
                              "seconds.",
                              root.string(), dirCount, fileCount, duration);
}

void FileSystemWalker::stop() { m_alive = false; }
