#include "file-indexer/file-indexer.hpp"
#include "file-service.hpp"

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

IndexerAsyncQuery *FileService::queryAsync(std::string_view query,
                                           const AbstractFileIndexer::QueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

FileService::FileService() { m_indexer = std::make_unique<FileIndexer>(); }
