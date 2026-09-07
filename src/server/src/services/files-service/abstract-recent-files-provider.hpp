#pragma once
#include <common/file-category.hpp>
#include <qfuture.h>
#include <filesystem>
#include <optional>
#include <vector>

struct RecentFilesParams {
  int limit = 50;
  std::optional<vicinae::FileCategory> category;
};

class AbstractRecentFilesProvider {
public:
  virtual bool isAvailable() const = 0;
  virtual QFuture<std::vector<std::filesystem::path>> listAsync(const RecentFilesParams &params) = 0;
  virtual void recordAccess(const std::filesystem::path &) {}

  virtual ~AbstractRecentFilesProvider() = default;
};
