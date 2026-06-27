#pragma once
#include "common/entrypoint.hpp"
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <string_view>
#include <vector>

class VisitTracker {
public:
  struct VisitInfo {
    std::optional<std::uint64_t> lastVisitedAt;
    int visitCount = 0;
  };

  struct QueryUsageInfo {
    std::optional<std::uint64_t> lastVisitedAt;
    int visitCount = 0;
  };

  struct Data {
    std::unordered_map<std::string, VisitInfo> visited;
    struct UsageEvent {
      std::string itemId;
      std::string query;
      std::uint64_t visitedAt;
    };

    std::vector<UsageEvent> usageEvents;
  };

  VisitTracker(const std::filesystem::path &path);

  void registerVisit(const EntrypointId &id, std::string_view query = {});
  void forget(const EntrypointId &id);
  VisitInfo getVisit(const EntrypointId &id) const;
  QueryUsageInfo getRollingUsage(const EntrypointId &id) const;
  QueryUsageInfo getQueryUsage(std::string_view query, const EntrypointId &id) const;

private:
  static constexpr std::uint64_t ROLLING_WINDOW_SECONDS = 28ULL * 24ULL * 60ULL * 60ULL;

  void loadFromDisk();
  void saveToDisk();
  void rebuildRollingUsageIndex();
  void pruneUsageEvents();

  std::filesystem::path m_path;
  std::string m_buf;
  Data m_data;
  std::unordered_map<std::string, QueryUsageInfo> m_rollingUsageByItem;
  std::unordered_map<std::string, std::unordered_map<std::string, QueryUsageInfo>> m_queryUsageByText;
};
