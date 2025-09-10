#pragma once
#include <filesystem>
#include <optional>

// Add new types here for new scanner types (e.g. watchers)
enum ScanType { Full, Incremental };

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

  bool operator<(const Scan &other) const {
    // TODO: Find a proper way to suppport std::set
    return path < other.path;
  }
};
