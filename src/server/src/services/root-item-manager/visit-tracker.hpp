#pragma once
#include "common/entrypoint.hpp"
#include <cstdint>
#include <unordered_map>
#include <filesystem>

class VisitTracker {
public:
  struct VisitInfo {
    std::optional<std::uint64_t> lastVisitedAt;
    int visitCount = 0;
  };

  struct Data {
    std::unordered_map<std::string, VisitInfo> visited;
  };

  VisitTracker(const std::filesystem::path &path);

  void registerVisit(const EntrypointId &id);
  void forget(const EntrypointId &id);
  VisitInfo getVisit(const EntrypointId &id) const;

private:
  void loadFromDisk();
  void saveToDisk();

  std::filesystem::path m_path;
  std::string m_buf;
  Data m_data;
};
