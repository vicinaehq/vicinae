#pragma once
#include "everything/everything-file-indexer.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "wsearch/win-file-indexer.hpp"

class WinFileIndexerSelector : public AbstractFileIndexer {
public:
  void start() override {}
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &preferences) override;
  bool isAvailable() const override;
  std::chrono::milliseconds queryDebounce() const override;

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const IndexerQueryParams &params = {}) override;

private:
  enum class Backend { Auto, WindowsSearch, Everything };

  AbstractFileIndexer &active();
  const AbstractFileIndexer &active() const;

  Backend m_backend = Backend::Auto;
  WinFileIndexer m_windowsSearch;
  EverythingFileIndexer m_everything;
};
