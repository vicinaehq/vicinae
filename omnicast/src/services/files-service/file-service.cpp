#include "file-indexer/file-indexer.hpp"
#include "file-service.hpp"

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

std::vector<AbstractFileIndexer::FileResult> FileService::search(std::string_view query) const {
  return m_indexer->query(query);
}

FileService::FileService() { m_indexer = std::make_unique<FileIndexer>(); }
