#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <QDebug>

class AbstractScanner {
  /*
   * Runs a scanner in its own thread, calls `finishCallback` when finished.
   * `finish()` and `fail()` are internal functions that update the DB and call `finishCallback`.
   *
   * `interrupt()` attempts to stop scanning.
   * `join()` blocks until everything has stopped, and the object can be deconstructed.
   */

public:
  using FinishCallback = std::function<void(ScanStatus)>;

private:
  int m_recordId;
  FinishCallback m_finishCallback;

protected:
  void finish() {
    m_db.updateScanStatus(m_recordId, ScanStatus::Succeeded);
    m_finishCallback(ScanStatus::Succeeded);
  }

  void fail() {
    m_db.updateScanStatus(m_recordId, ScanStatus::Failed);
    m_finishCallback(ScanStatus::Failed);
  }

public:
  FileIndexerDatabase m_db;

  AbstractScanner(const Scan &scan, FinishCallback callback) : m_finishCallback(callback) {

    auto result = m_db.createScan(scan.path, scan.type);

    if (!result.has_value()) {
      qWarning() << "Not scanning" << scan.path.native() << "because scan record creation failed with error"
                 << result.error();
      // TODO: Signal that the scan failed to start
      m_finishCallback(ScanStatus::Failed);
      return;
    }

    m_db.updateScanStatus(m_recordId, ScanStatus::Started);
  }
  virtual ~AbstractScanner() = default;

  virtual void interrupt() { m_db.updateScanStatus(m_recordId, ScanStatus::Interrupted); }
  virtual void join() = 0;
};
