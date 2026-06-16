#pragma once
#include "file-indexer/util.hpp"
#include "file-indexer/abstract-scanner.hpp"
#include "file-indexer/filesystem-walker.hpp"
#include <atomic>
#include <memory>
#include <thread>

class IndexerScanner : public AbstractScanner, public file_indexer::NonCopyable {
public:
  IndexerScanner(const std::shared_ptr<DbWriter> &writer, const Scan &scan, StatusCallback callback);
  ~IndexerScanner() override = default;

  void interrupt() override;
  void join() override;

private:
  static constexpr size_t INDEX_BATCH_SIZE = 5'000;

  void scan(const Scan &scan);

  std::atomic<bool> m_alive = true;
  FileSystemWalker m_walker;
  std::thread m_scanThread;
};
