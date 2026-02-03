#pragma once
#include "services/files-service/file-indexer/abstract-scanner.hpp"
#include "watcher.hpp"

class WatcherScanner : public AbstractScanner {
private:
  std::unique_ptr<wtr::watch> m_watch;
  Scan scan;

  void handleMessage(const wtr::event &ev);
  void handleEvent(const wtr::event &ev);

public:
  WatcherScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, FinishCallback callback);
  void interrupt() override;
  void join() override;
};
