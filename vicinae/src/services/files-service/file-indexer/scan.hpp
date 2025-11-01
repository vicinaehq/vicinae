#pragma once
#include <filesystem>
#include <optional>
#include <vector>

// Add new types here for new scanner types (e.g. watchers)
enum ScanType { Full, Incremental, Watcher };

enum ScanStatus {
  Pending,
  Started,
  Interrupted,
  Failed,
  Succeeded,
};

// TODO: use unions for each ScanType
struct Scan {
  ScanType type;
  std::filesystem::path path;
  std::optional<size_t> maxDepth;
  std::vector<std::string> excludedFilenames;
  std::vector<std::filesystem::path> excludedPaths;

  bool operator<(const Scan &other) const {
    // TODO: Find a proper way to suppport std::set
    return path < other.path;
  }
};

enum class FileEventType { Modify, Delete };

struct FileEvent {
  FileEventType type;
  std::filesystem::path path;
  std::filesystem::file_time_type eventTime;
};
