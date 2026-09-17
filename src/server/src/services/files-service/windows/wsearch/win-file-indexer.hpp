#pragma once
#include "services/files-service/abstract-file-indexer.hpp"

class WinFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  bool isAvailable() const override;
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &) override {}

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {}) override;
};
