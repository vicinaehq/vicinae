#pragma once
#include "everything-file-indexer.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "win-file-indexer.hpp"

// Picks between Windows Search and Everything according to the file extension preferences.
class WinFileIndexerSelector : public AbstractFileIndexer {
public:
  void start() override {}
  void rebuildIndex() override {}
  void preferenceValuesChanged(const QJsonObject &preferences) override;
  bool isAvailable() const override;

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
