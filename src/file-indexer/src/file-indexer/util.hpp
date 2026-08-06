#pragma once
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <limits>
#include <optional>
#include <system_error>
#include <vector>
#include "file-indexer/log.hpp"
#include "xdgpp/env/env.hpp"

namespace file_indexer {

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;
  NonCopyable() = default;
};

inline std::filesystem::path homeDir() {
  const char *env = std::getenv("HOME");
  if (!env) return {};
  return env;
}

inline std::filesystem::path cacheDir() { return xdgpp::cacheHome() / "vicinae" / "file-indexer"; }

inline std::filesystem::path databasePath() { return cacheDir() / "file-indexer.db"; }

inline std::filesystem::path normalizePath(const std::filesystem::path &path) {
  return std::filesystem::absolute(path).lexically_normal();
}

inline std::vector<std::filesystem::path> normalizePaths(std::vector<std::filesystem::path> paths) {
  for (auto &path : paths) {
    path = normalizePath(path);
  }

  std::ranges::sort(paths);
  auto [first, last] = std::ranges::unique(paths);
  paths.erase(first, last);

  return paths;
}

inline bool isSameOrDescendantOf(const std::filesystem::path &path, const std::filesystem::path &ancestor) {
  auto pathIt = path.begin();
  auto ancestorIt = ancestor.begin();

  for (; ancestorIt != ancestor.end(); ++ancestorIt, ++pathIt) {
    if (pathIt == path.end() || *pathIt != *ancestorIt) return false;
  }

  return true;
}

inline bool isCoveredByAny(const std::filesystem::path &path,
                           const std::vector<std::filesystem::path> &roots) {
  return std::ranges::any_of(
      roots, [&](const std::filesystem::path &root) { return isSameOrDescendantOf(path, root); });
}

inline std::vector<std::filesystem::path> compactSubtrees(std::vector<std::filesystem::path> paths) {
  std::ranges::sort(paths, [](const std::filesystem::path &lhs, const std::filesystem::path &rhs) {
    auto const lhsSize = std::ranges::distance(lhs);
    auto const rhsSize = std::ranges::distance(rhs);
    if (lhsSize != rhsSize) return lhsSize < rhsSize;
    return lhs < rhs;
  });

  std::vector<std::filesystem::path> compacted;
  compacted.reserve(paths.size());

  for (const auto &path : paths) {
    if (!isCoveredByAny(path, compacted)) { compacted.emplace_back(path); }
  }

  return compacted;
}
inline std::optional<int64_t> fileSizeBytesFor(const std::filesystem::path &path, bool isDirectory) {
  if (isDirectory) return std::nullopt;

  std::error_code ec;
  auto const size = std::filesystem::file_size(path, ec);

  if (ec || size > static_cast<uintmax_t>(std::numeric_limits<int64_t>::max())) { return std::nullopt; }

  return static_cast<int64_t>(size);
}

inline void removeLegacyDbFiles() {
  std::error_code ec;
  auto paths = {
      xdgpp::dataHome() / "vicinae" / "file-indexer.db",
      xdgpp::dataHome() / "vicinae" / "file-indexer" / "file-indexer.db",
  };

  for (const auto &path : paths) {
    auto files = {std::format("{}-wal", path.c_str()), std::format("{}-shm", path.c_str()),
                  std::string{path.c_str()}};

    for (const auto &file : files) {
      if (std::filesystem::remove(file, ec)) { flog::info() << "Removed " << file << "\n"; }
    }
  }
}

inline void purgeDbFiles() {
  auto dir = databasePath();
  auto files = {std::format("{}-wal", dir.c_str()), std::format("{}-shm", dir.c_str()),
                std::string{dir.c_str()}};
  std::error_code ec;

  for (const auto &file : files) {
    if (std::filesystem::remove(file, ec)) { flog::info() << "Removed " << file << "\n"; }
  }
}

inline std::filesystem::path downloadsFolder() { return homeDir() / "Downloads"; }
inline std::filesystem::path documentsFolder() { return homeDir() / "Documents"; }

inline bool isHiddenPath(const std::filesystem::path &path) {
  return std::ranges::any_of(path, [](auto &&part) { return part.string().starts_with('.'); });
}

inline bool isInHomeDirectory(const std::filesystem::path &path) {
  return path.string().starts_with(homeDir().string());
}

inline std::string getLastPathComponent(const std::filesystem::path &path) {
  if (!path.has_filename()) {
    if (path.has_parent_path()) return path.parent_path().filename();
    return path;
  }
  return path.filename();
}

}; // namespace file_indexer
