#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include <string_view>

class FileService {
  std::unique_ptr<AbstractFileIndexer> m_indexer;

public:
  AbstractFileIndexer *indexer() const;

  void rebuildIndex();

  QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view query,
                                                     const AbstractFileIndexer::QueryParams &params = {});

  void setEntrypoints(const std::vector<AbstractFileIndexer::Entrypoint> &entrypoints);

  FileService();
};
