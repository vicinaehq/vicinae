#pragma once
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <system_error>
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

inline std::filesystem::path dataDir() { return xdgpp::dataHome() / "vicinae" / "file-indexer"; }

inline std::filesystem::path databasePath() { return dataDir() / "file-indexer.db"; }

inline void removeLegacyDbFiles() {
  std::error_code ec;
  auto dir = xdgpp::dataHome() / "vicinae" / "file-indexer.db";
  auto files = {std::format("{}-wal", dir.c_str()), std::format("{}-shm", dir.c_str()),
                std::string{dir.c_str()}};

  for (const auto &file : files) {
    if (std::filesystem::remove(file, ec)) { flog::info() << "Removed " << file << "\n"; }
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
