#pragma once
#include <filesystem>
#include <optional>

// Add new types here for new scanner types (e.g. watchers)
enum ScanType {
  Full,
  Incremental
};

// TODO: use unions for each ScanType
struct Scan {
  ScanType type;
  std::filesystem::path path;
  std::optional<size_t> maxDepth;
};
