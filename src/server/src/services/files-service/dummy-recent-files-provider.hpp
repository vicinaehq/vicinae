#pragma once
#include "services/files-service/abstract-recent-files-provider.hpp"

class DummyRecentFilesProvider : public AbstractRecentFilesProvider {
public:
  bool isAvailable() const override { return false; }

  QFuture<std::vector<std::filesystem::path>> listAsync(const RecentFilesParams &) override {
    return QtFuture::makeReadyValueFuture(std::vector<std::filesystem::path>{});
  }
};
