#pragma once
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include "watcher.hpp"
#include <vector>

class WatcherScanner : public AbstractScanner {
private:
  FileIndexerDatabase m_db;

  struct Element {
    FileIndexerDatabase::ScanRecord m_record;
    std::unique_ptr<wtr::watch> m_watch;
    bool m_failed;
  };
  std::vector<Element> m_watches;

  void handleMessage(size_t index, const wtr::event &ev);
  void handleEvent(size_t index, const wtr::event &ev);

public:
  void run() override;
  void stop() override;
};
