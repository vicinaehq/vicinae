#pragma once
#include "services/files-service/abstract-recent-files-provider.hpp"

class WinRecentFilesProvider : public AbstractRecentFilesProvider {
public:
  bool isAvailable() const override { return true; }
  QFuture<std::vector<std::filesystem::path>> listAsync(const RecentFilesParams &params) override;
  void recordAccess(const std::filesystem::path &path) override;
};
