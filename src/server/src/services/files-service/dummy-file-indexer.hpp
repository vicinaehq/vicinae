#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include <QtConcurrent/QtConcurrent>

class DummyFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &) override {}

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view,
                                                     const IndexerQueryParams & = {}) override {
    return QtFuture::makeReadyValueFuture(std::vector<IndexerFileResult>{});
  }

  bool isAvailable() const override { return false; }
};
