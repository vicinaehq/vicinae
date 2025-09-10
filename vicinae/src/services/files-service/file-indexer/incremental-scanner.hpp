#pragma once
#include "common.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"

class IncrementalScanner : public AbstractScanner, public NonCopyable {
  std::thread m_scanThread;

  std::vector<std::filesystem::path> getScannableDirectories(const std::filesystem::path &path,
                                                             std::optional<size_t> maxDepth) const;
  void processDirectory(const std::filesystem::path &path);

  void scan(const Scan &scan);

public:
  IncrementalScanner(const Scan &scan, FinishCallback callback);
  ~IncrementalScanner() = default;

  void interrupt() override;
  void join() override;
};
