#pragma once
#include "services/files-service/abstract-file-indexer.hpp"

class FileService {
  std::unique_ptr<AbstractFileIndexer> m_indexer;

public:
  AbstractFileIndexer *indexer() const;
  std::vector<AbstractFileIndexer::FileResult> search(std::string_view query) const;

  FileService();
};
