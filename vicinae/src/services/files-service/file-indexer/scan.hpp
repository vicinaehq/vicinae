#pragma once
#include <filesystem>
#include <optional>

enum ScanStatus {
  Pending,
  Started,
  Failed,
  Finished
};

enum ScanType {
  Full,
  Incremental
};

struct Scan {
  ScanType type;
  std::filesystem::path path;
  std::optional<size_t> maxDepth;
};
