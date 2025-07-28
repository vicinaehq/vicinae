#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <qsqldatabase.h>
#include <filesystem>

class IncrementalScanner : public NonCopyable {
  FileIndexerDatabase &m_db;

  std::vector<std::filesystem::path> getScannableDirectories(const std::filesystem::path &path,
                                                             std::optional<size_t> maxDepth) const;
  void processDirectory(const std::filesystem::path &path);

public:
  void scan(const std::filesystem::path &path, std::optional<size_t> maxDepth);
  IncrementalScanner(FileIndexerDatabase &db);
};
