#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"

class IncrementalScanner : public AbstractScanner, public NonCopyable {
  std::unique_ptr<FileIndexerDatabase> m_read_db;

  std::thread m_scanThread;

  std::vector<std::filesystem::path> getScannableDirectories(const std::filesystem::path &path,
                                                             std::optional<size_t> maxDepth) const;
  void processDirectory(const std::filesystem::path &path);

  void scan(const Scan &scan);

public:
  IncrementalScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, FinishCallback callback);
  ~IncrementalScanner() = default;

  void interrupt() override;
  void join() override;
};
