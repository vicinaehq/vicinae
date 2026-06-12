#pragma once
#include <filesystem>
#include <optional>
#include <vector>

enum ScanType { Full, Incremental };

enum class ScanMode {
  Exhaustive,
  // only descends into new or modified-since-last-scan directories: misses
  // changes deep under unchanged ancestors
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
  bool isDirectory = false;
};
