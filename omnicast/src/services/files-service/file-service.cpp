#include "file-indexer/file-indexer.hpp"
#include "file-service.hpp"

AbstractFileIndexer *FileService::indexer() const { return m_indexer.get(); }

FileService::FileService() { m_indexer = std::make_unique<FileIndexer>(); }
