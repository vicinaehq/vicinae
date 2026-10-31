#pragma once
#include "services/files-service/abstract-recent-files-provider.hpp"

class XbelRecentFilesProvider : public AbstractRecentFilesProvider {
public:
  XbelRecentFilesProvider();

  bool isAvailable() const override;
  QFuture<std::vector<std::filesystem::path>> listAsync(const RecentFilesParams &params) override;
  void recordAccess(const std::filesystem::path &path) override;

private:
  std::filesystem::path m_xbelPath;
};
