#include "services/files-service/file-indexer/filesystem-walker.hpp"
#include "utils/utils.hpp"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <fnmatch.h>
#include <qlogging.h>
#include <qobjectdefs.h>
#include <stack>
#include <qlogging.h>
#include <string>

namespace fs = std::filesystem;

/*
 * List of absolute paths to never follow during indexing. Mostly used to exclude
 * pseudo filesystems such as /run or /proc.
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::vector<fs::path> EXCLUDED_PATHS = {"/sys", "/run",     "/proc", "/tmp",

                                                     "/mnt", "/var/tmp", "/efi",  "/dev"};

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
    std::string filename = getLastPathComponent(path);
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

void FileSystemWalker::setRecursive(bool value) { m_recursive = value; }

void FileSystemWalker::setMaxDepth(std::optional<size_t> maxDepth) { m_maxDepth = maxDepth; }

void FileSystemWalker::setIgnoreHiddenPaths(bool value) { m_ignoreHiddenFiles = value; }

bool FileSystemWalker::isIgnored(const std::filesystem::path &path) const {
  fs::path p = path.parent_path();

  while (p != p.root_directory()) {
    for (const auto &name : m_ignoreFiles) {
      fs::path ignorePath = p / name;

      if (!fs::is_regular_file(ignorePath)) continue;
      if (GitIgnoreReader(ignorePath).matches(path)) { return true; }
    }

    p = p.parent_path();
  }

  return false;
}

void FileSystemWalker::walk(const fs::path &root, const WalkCallback &callback) {
  std::stack<fs::path> dirStack;
  size_t rootDepth = std::distance(root.begin(), root.end());

  dirStack.push(root);

  while (!dirStack.empty()) {
    auto path = dirStack.top();
    std::error_code ec;

    dirStack.pop();

    for (const auto &entry : fs::directory_iterator(path, ec)) {
      if (ec) {
        qDebug() << "walk error" << ec.message();
        continue;
      }
      if (entry.is_symlink(ec)) continue;

      auto path = entry.path();

      if (m_ignoreHiddenFiles && isHiddenPath(path)) continue;
      if (std::ranges::contains(EXCLUDED_PATHS, path)) { continue; }
      if (std::ranges::contains(EXCLUDED_FILENAMES, path.filename())) { continue; }
      if (isIgnored(path)) { continue; }

      if (m_recursive && entry.is_directory()) {
        size_t depth = std::distance(path.begin(), path.end()) - rootDepth;

        if (!(m_maxDepth && depth > *m_maxDepth)) { dirStack.push(entry); }
      }

      callback(entry);
    }
  }
}
