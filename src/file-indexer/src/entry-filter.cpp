#include "file-indexer/entry-filter.hpp"
#include "file-indexer/util.hpp"
#include "file-indexer/vocabulary.hpp"
#include <algorithm>
#include <array>
#include <fstream>
#include <fnmatch.h>
#include <ranges>
#include <string>
#include <unordered_set>
#include <utility>

namespace fs = std::filesystem;

static const std::unordered_set<std::string> &excludedPaths() {
  static const std::unordered_set<std::string> paths = [] {
    std::unordered_set<std::string> set = {"/sys", "/run", "/proc", "/tmp", "/var/tmp", "/efi", "/dev"};

    const auto HOME_RELATIVE = {
        ".local/share/Trash",
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
        ".local/share/flatpak/app",
        ".local/share/flatpak/repo/objects",
        ".local/share/flatpak/runtime",
        ".local/share/pnpm/store",
        ".local/share/Steam/config/htmlcache",
        ".local/share/vicinae/clipboard-data",
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

static constexpr auto EXCLUDED_FILENAMES = std::array{
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

static bool pathContains(const fs::path &path, std::string_view component) {
  return std::ranges::any_of(path, [&](const fs::path &part) { return part.native() == component; });
}

static bool pathContainsAfter(const fs::path &path, std::string_view first, std::string_view second) {
  bool seenFirst = false;

  for (const auto &part : path) {
    const auto name = part.native();
    if (seenFirst && name == second) return true;
    if (name == first) seenFirst = true;
  }

  return false;
}

static bool isMachineTrashDirectory(const fs::path &path) {
  const auto filename = file_indexer::vocab::basenameView(path.c_str());

  if (filename == "Cache_Data") return pathContains(path, "Cache");
  if (filename == "GPUCache") return true;
  if (filename == "Code Cache") return true;
  if (filename == "CacheStorage") return true;
  if (filename == "Service Worker") return true;
  if (filename == "blob_storage") return true;
  if (filename == "DawnGraphiteCache") return true;
  if (filename == "DawnWebGPUCache") return true;
  if (filename == "mesa_shader_cache") return true;
  if (filename == "shader_cache") return true;
  if (filename == "ShaderCache") return true;
  if (filename == "GrShaderCache") return true;
  if (filename == "Crashpad") return true;
  if (filename == "crashpad") return true;
  if (filename == "cache" && pathContains(path, "Shared Dictionary")) return true;

  if (pathContainsAfter(path, ".var", "cache")) return true;

  return false;
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
  const auto filename = file_indexer::vocab::basenameView(path.c_str());

  if (std::ranges::contains(EXCLUDED_FILENAMES, filename) || filename.ends_with(".noindex")) return false;
  if (std::ranges::find(m_excludedFilenames, filename) != m_excludedFilenames.end()) return false;
  if (entry.is_directory(ec) && isMachineTrashDirectory(path)) return false;

  if (isIgnored(path)) return false;
  if (isExcludedPath(path)) return false;

  return true;
}

bool EntryFilter::isCacheDirTag(const fs::path &entryPath) {
  return entryPath.native().ends_with(CACHEDIR_TAG_SUFFIX) && hasCacheDirTagSignature(entryPath);
}
