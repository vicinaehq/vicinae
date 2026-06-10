#include "file-indexer/filesystem-walker.hpp"
#include "file-indexer/log.hpp"
#include "file-indexer/util.hpp"
#include <array>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <fnmatch.h>
#include <stack>
#include <string>
#include <unordered_set>

namespace fs = std::filesystem;

/*
 * Absolute paths to never follow during indexing: pseudo filesystems such as /run or /proc,
 * plus well-known locations under the user's home that only ever hold machine-managed data
 * (package caches, toolchains, container/image layers, game libraries...).
 * Contextual exclusions (using gitignore-like semantics) are handled separately.
 */
static const std::unordered_set<std::string> &excludedPaths() {
  static const std::unordered_set<std::string> paths = [] {
    std::unordered_set<std::string> set = {"/sys", "/run", "/proc", "/tmp", "/var/tmp", "/efi", "/dev"};

    const auto HOME_RELATIVE = {
        ".local/share/Trash",
        ".local/share/Steam/steamapps",
        ".local/share/Steam/steamrt64",
        ".local/share/Steam/ubuntu12_32",
        ".local/share/Steam/ubuntu12_64",
        ".local/share/flatpak",
        ".local/share/containers",
        ".local/share/icons",
        ".local/share/pnpm",
        ".local/share/JetBrains",
        ".local/share/zed",
        ".icons",
        ".conda",
        "anaconda3",
        "miniconda3",
        "miniforge3",
        ".cargo/registry",
        ".cargo/git",
        ".rustup",
        "go/pkg",
        ".go/pkg",
        ".bun/install",
        ".npm",
        ".nvm",
        ".pnpm-store",
        ".yarn",
        ".m2/repository",
        ".ivy2",
        ".wine",
        ".docker",
        ".android",
        "snap",
    };

    if (auto home = file_indexer::homeDir(); !home.empty()) {
      for (const auto &rel : HOME_RELATIVE) {
        set.insert((home / rel).native());
      }
    }

    return set;
  }();

  return paths;
}

/**
 * Filenames that can always be ignored. If any file names are to be added here, it's important to make sure
 * they have a specific enough name so that it doesn't generate false positives and prevent indexing actually
 * meaningful content. If you are not sure, it's better to not add it.
 *
 * The indexer is pretty fast and can index millions of files without issue, so indexing some garbage
 * is forgivable.
 */
static const std::unordered_set<std::string_view> EXCLUDED_FILENAMES = {
    ".git",
    ".hg",
    ".svn",
    ".cache",
    ".clangd",
    ".ccache",
    ".gradle",
    ".terraform",
    "__pycache__",
    ".venv",
    "venv",
    ".tox",
    ".mypy_cache",
    ".pytest_cache",
    ".ruff_cache",
    "node_modules",
    ".next",
    ".nuxt",
    ".turbo",
    ".parcel-cache",
    ".angular",
    ".svelte-kit",
    "GPUCache",
    "Code Cache",
    "CacheStorage",
    "Service Worker",
    "blob_storage",
    "DawnGraphiteCache",
    "DawnWebGPUCache",
    "__MACOSX",
    ".Trash",
    "lost+found",
    ".snapshots",
};

static constexpr std::string_view CACHEDIR_TAG_SUFFIX = "/CACHEDIR.TAG";
static constexpr std::string_view CACHEDIR_TAG_SIGNATURE = "Signature: 8a477f597d28d172789f06886806bc55";

/*
 * Cache Directory Tagging Specification: directories containing a valid CACHEDIR.TAG
 * file hold machine-generated cache data and should not be indexed.
 * Notably written by cargo in every target/ directory.
 */
static bool hasCacheDirTagSignature(const fs::path &path) {
  std::ifstream ifs(path, std::ios::binary);
  std::array<char, CACHEDIR_TAG_SIGNATURE.size()> buf{};

  if (!ifs.read(buf.data(), buf.size())) return false;

  return std::string_view(buf.data(), buf.size()) == CACHEDIR_TAG_SIGNATURE;
}

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

      if (entry.path().native().ends_with(CACHEDIR_TAG_SUFFIX) && hasCacheDirTagSignature(entry.path())) {
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
      if (entry.is_symlink(ec)) { continue; }

      const auto &path = entry.path();

      if (m_ignoreHiddenFiles && file_indexer::isHiddenPath(path)) {
        if (m_verbose) { flog::info() << "FileSystemWalker: ignoring hidden path" << path.c_str(); }
        continue;
      }

      if (excludedPaths().contains(path.native())) { continue; }

      // .noindex is the Spotlight convention for "do not index", honored by many apps
      const std::string filename = path.filename().native();
      if (EXCLUDED_FILENAMES.contains(filename) || filename.ends_with(".noindex")) { continue; }
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
