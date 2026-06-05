#pragma once
#include "services/files-service/abstract-file-indexer.hpp"

/**
 * macOS file indexer backed by Spotlight. Spotlight owns the index, so there is
 * nothing to start, rebuild or configure: only querying is implemented. Spotlight
 * acts as a candidate generator (substring match on the file name) and results are
 * re-ranked with the same fuzzy matcher used by the Linux indexer.
 */
class SpotlightFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &) override {}

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const QueryParams &params = {}) override;
};
