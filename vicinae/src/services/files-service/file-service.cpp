#include "file-indexer/file-indexer.hpp"
#include "services/files-service/abstract-file-indexer.hpp"
#include "file-service.hpp"

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

QFuture<std::vector<IndexerFileResult>>
FileService::queryAsync(std::string_view query, const AbstractFileIndexer::QueryParams &params) {
  return m_indexer->queryAsync(query, params);
}

void FileService::rebuildIndex() { m_indexer->rebuildIndex(); }

FileService::FileService() { m_indexer = std::make_unique<FileIndexer>(); }
