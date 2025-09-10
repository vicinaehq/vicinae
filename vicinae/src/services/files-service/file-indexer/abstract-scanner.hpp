#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <QDebug>
#include <memory>

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
    m_db->updateScanStatus(m_recordId, ScanStatus::Succeeded);
    m_finishCallback(ScanStatus::Succeeded);
  }

  void fail() {
    m_db->updateScanStatus(m_recordId, ScanStatus::Failed);
    m_finishCallback(ScanStatus::Failed);
  }

  void dbAtThread() {
    m_db = std::make_unique<FileIndexerDatabase>();
  }

public:
  std::unique_ptr<FileIndexerDatabase> m_db;

  AbstractScanner(const Scan &scan, FinishCallback callback) : m_finishCallback(callback) {
    m_db = std::make_unique<FileIndexerDatabase>();
    auto result = m_db->createScan(scan.path, scan.type);

    if (!result.has_value()) {
      qWarning() << "Not scanning" << scan.path.native() << "because scan record creation failed with error"
                 << result.error();
      // TODO: Signal that the scan failed to start
      m_finishCallback(ScanStatus::Failed);
      return;
    }

    m_recordId = result->id;
    m_db->updateScanStatus(m_recordId, ScanStatus::Started);
  }
  virtual ~AbstractScanner() = default;

  virtual void interrupt() { m_db->updateScanStatus(m_recordId, ScanStatus::Interrupted); }
  virtual void join() = 0;
};
