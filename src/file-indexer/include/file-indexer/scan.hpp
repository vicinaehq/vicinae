#pragma once
#include <filesystem>
#include <optional>
#include <vector>

enum ScanType { Full, Incremental };

enum class ScanMode {
  // visit every directory under the scan path (subject to maxDepth)
  Exhaustive,
  // only descend into directories that are new or were modified since the last
  // successful scan, recursively. Cheap, but changes deep under unchanged
  // directories are not discovered.
  Pruned
};

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
  ScanMode mode = ScanMode::Exhaustive;
  std::filesystem::path path;
  std::optional<size_t> maxDepth;
  std::vector<std::string> excludedFilenames;
  std::vector<std::filesystem::path> excludedPaths;
  bool notify = false;

  bool operator==(const Scan &) const = default;

  bool operator<(const Scan &other) const {
    // TODO: Find a proper way to suppport std::set
    return path < other.path;
  }
};

struct ScanEvent {
  int scanId;
  ScanType type;
  ScanStatus status;
  std::filesystem::path entrypoint;
  size_t processedFileCount;
};

enum class FileEventType { Modify, Delete };

struct FileEvent {
  FileEventType type;
  std::filesystem::path path;
  std::filesystem::file_time_type eventTime;
};
