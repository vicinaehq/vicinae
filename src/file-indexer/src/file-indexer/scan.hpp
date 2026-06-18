#pragma once
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>
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

struct FullScan {
  std::vector<std::filesystem::path> excludedPaths;

  bool operator==(const FullScan &rhs) const = default;
};

struct IncrementalScan {
  ScanMode mode = ScanMode::Exhaustive;
  std::optional<size_t> maxDepth;
  std::vector<std::string> excludedFilenames;
  std::vector<std::filesystem::path> excludedPaths;

  bool operator==(const IncrementalScan &rhs) const = default;
};

struct Scan {
  std::filesystem::path path;
  std::variant<FullScan, IncrementalScan> data;
  bool notify = false;

  bool operator==(const Scan &rhs) const = default;

  ScanType type() const {
    return std::holds_alternative<FullScan>(data) ? ScanType::Full : ScanType::Incremental;
  }

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
  std::optional<int64_t> sizeBytes;
};
