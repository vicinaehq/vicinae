#pragma once
#include "services/files-service/abstract-file-indexer.hpp"
#include <string_view>

class FileService {
  std::unique_ptr<AbstractFileIndexer> m_indexer;

public:
  AbstractFileIndexer *indexer() const;
  IndexerAsyncQuery *queryAsync(std::string_view query, const AbstractFileIndexer::QueryParams &params = {});

  FileService();
};
