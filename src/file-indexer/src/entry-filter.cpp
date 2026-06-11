#include "file-indexer/entry-filter.hpp"
#include "file-indexer/util.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <fnmatch.h>
#include <string>
#include <unordered_set>
#include <utility>

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

void EntryFilter::setExcludedPaths(std::vector<fs::path> paths) { m_excludedPaths = std::move(paths); }

void EntryFilter::setExcludedFilenames(std::vector<std::string> filenames) {
  m_excludedFilenames = std::move(filenames);
}

void EntryFilter::setIgnoreFiles(std::vector<std::string> files) { m_ignoreFiles = std::move(files); }

void EntryFilter::setIgnoreHiddenPaths(bool value) { m_ignoreHiddenPaths = value; }

bool EntryFilter::isIgnored(const fs::path &path) const {
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

bool EntryFilter::isExcludedPath(const fs::path &path) const {
  return std::ranges::find(m_excludedPaths, path) != m_excludedPaths.end();
}

bool EntryFilter::shouldVisit(const fs::directory_entry &entry) const {
  std::error_code ec;

  if (entry.is_symlink(ec)) return false;

  const auto &path = entry.path();

  if (m_ignoreHiddenPaths && file_indexer::isHiddenPath(path)) return false;
  if (excludedPaths().contains(path.native())) return false;

  // .noindex is the Spotlight convention for "do not index", honored by many apps
  const std::string filename = path.filename().native();
  if (EXCLUDED_FILENAMES.contains(filename) || filename.ends_with(".noindex")) return false;
  if (std::ranges::find(m_excludedFilenames, filename) != m_excludedFilenames.end()) return false;

  if (isIgnored(path)) return false;
  if (isExcludedPath(path)) return false;

  return true;
}

bool EntryFilter::isCacheDirTag(const fs::path &entryPath) {
  return entryPath.native().ends_with(CACHEDIR_TAG_SUFFIX) && hasCacheDirTagSignature(entryPath);
}
