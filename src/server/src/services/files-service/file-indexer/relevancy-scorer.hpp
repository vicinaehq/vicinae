#pragma once
#include <filesystem>
#include <optional>

class RelevancyScorer {
  double computeLocationMultiplier(const std::filesystem::path &path);
  double computeFileTypeMultiplier(const std::filesystem::path &path);
  double computeHiddenFileMultiplier(const std::filesystem::path &path);
  double computePathDepthMultiplier(const std::filesystem::path &path);
  double computeRecencyMultiplier(const std::filesystem::file_time_type lastModified);

public:
  double computeScore(const std::filesystem::path &path,
                      std::optional<std::filesystem::file_time_type> lastModified);
};
