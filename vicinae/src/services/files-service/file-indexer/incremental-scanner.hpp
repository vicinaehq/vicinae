#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include <memory>
#include <qsqldatabase.h>
#include <filesystem>

class IncrementalScanner : public AbstractScanner, public NonCopyable {
  std::unique_ptr<FileIndexerDatabase> m_db;

  std::vector<std::filesystem::path> getScannableDirectories(const std::filesystem::path &path,
                                                             std::optional<size_t> maxDepth) const;
  void processDirectory(const std::filesystem::path &path);

  void scan(const Scan &scan);

public:
  void run() override;
  void stop() override;
};
