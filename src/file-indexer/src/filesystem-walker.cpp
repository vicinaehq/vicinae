#include "file-indexer/filesystem-walker.hpp"
#include "file-indexer/log.hpp"
#include <chrono>
#include <filesystem>
#include <format>
#include <stack>
#include <string>
#include <vector>

namespace fs = std::filesystem;

void FileSystemWalker::setIgnoreFiles(const std::vector<std::string> &files) {
  m_filter.setIgnoreFiles(files);
}

void FileSystemWalker::setExcludedPaths(const std::vector<std::filesystem::path> &paths) {
  m_filter.setExcludedPaths(paths);
}

void FileSystemWalker::setRecursive(bool value) { m_recursive = value; }

void FileSystemWalker::setMaxDepth(std::optional<size_t> maxDepth) { m_maxDepth = maxDepth; }

void FileSystemWalker::setIgnoreHiddenPaths(bool value) { m_filter.setIgnoreHiddenPaths(value); }

void FileSystemWalker::setVerbose(bool value) { m_verbose = value; }

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

  // Reused across directories to avoid reallocating for every one of them
  std::vector<fs::directory_entry> entries;

  dirStack.push(root);

  while (!dirStack.empty()) {
    if (!m_alive) break;

    auto dir = dirStack.top();

    dirStack.pop();

    entries.clear();
    bool isCacheDir = false;

    for (const auto &entry : fs::directory_iterator(dir, ec)) {
      if (ec) {
        flog::warn() << "walk error" << ec.message().c_str();
        continue;
      }

      if (EntryFilter::isCacheDirTag(entry.path())) {
        isCacheDir = true;
        break;
      }

      entries.emplace_back(entry);
    }

    if (isCacheDir) {
      if (m_verbose) { flog::info() << "FileSystemWalker: skipping CACHEDIR.TAG directory" << dir.c_str(); }
      continue;
    }

    for (const auto &entry : entries) {
      const auto &path = entry.path();

      m_pacer.checkpoint();

      if (!m_filter.shouldVisit(entry)) {
        if (m_verbose) { flog::info() << "FileSystemWalker: skipping filtered path" << path.c_str(); }
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
