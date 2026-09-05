#pragma once
#include <memory>
#include <mutex>
#include <string>
#include "everything-client.hpp"
#include "services/files-service/abstract-file-indexer.hpp"

// Query-only indexer backed by a running Everything (voidtools) instance.
class EverythingFileIndexer : public AbstractFileIndexer {
public:
  void start() override {}
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &) override {}
  bool isAvailable() const override;

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {}) override;

  void setInstanceName(std::wstring name);

private:
  bool ensureClient() const;
  std::vector<IndexerFileResult> runQuery(const std::string &query, const IndexerQueryParams &params);

  mutable std::mutex m_mutex;
  mutable std::unique_ptr<EverythingClient> m_client;
  std::wstring m_instanceName;
};
