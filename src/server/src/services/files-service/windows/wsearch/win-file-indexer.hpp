#pragma once
#include "services/files-service/abstract-file-indexer.hpp"

class WinFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  bool isAvailable() const override;
  void rebuildIndex() override {}
  void preferencesChanged(const FilePreferences &) override {}

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {}) override;
};
