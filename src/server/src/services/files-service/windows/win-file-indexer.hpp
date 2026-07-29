#pragma once
#include "services/files-service/abstract-file-indexer.hpp"

/**
 * Windows file indexer backed by the system Windows Search index (WSearch service,
 * SystemIndex catalog), queried through the Search.CollatorDSO OLE DB provider.
 */
class WinFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  bool isAvailable() const override;
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &) override {}

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {}) override;
};
