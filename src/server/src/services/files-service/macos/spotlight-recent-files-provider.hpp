#pragma once
#include "services/files-service/abstract-recent-files-provider.hpp"

/**
 * LaunchServices updates kMDItemLastUsedDate whenever a file is opened through
 * NSWorkspace, which is how Vicinae launches things, so nothing is recorded here.
 */
class SpotlightRecentFilesProvider : public AbstractRecentFilesProvider {
public:
  bool isAvailable() const override { return true; }
  QFuture<std::vector<std::filesystem::path>> listAsync(const RecentFilesParams &params) override;
};
