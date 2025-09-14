#pragma once
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <QDebug>
#include <memory>

class AbstractScanner {
  /*
   * Runs a scanner in its own thread, calls `finishCallback` when finished.
   * `finish()` and `fail()` are internal functions that update the DB and call `finishCallback`.
   * `setInterruptFlag()` makes `finish()` write `Interrupted` instead of `Finished`.
   * `dbAtThread()` re-constructs the database in the given thread - this is necessary.
   *
   * `interrupt()` signals the scanner to stop prematurely.
   * `join()` joins any threads and sets the scanner up for deconstruction.
   * This waits until the scan is finished - call `interrupt()` to stop faster.
   */

public:
  using FinishCallback = std::function<void(ScanStatus)>;

private:
  int m_recordId;
  FinishCallback m_finishCallback;
  std::atomic<bool> m_interrupted = false;

protected:
  std::unique_ptr<FileIndexerDatabase> m_db;

  void finish() {
    ScanStatus status = m_interrupted? ScanStatus::Interrupted: ScanStatus::Succeeded;
    m_db->updateScanStatus(m_recordId, status);
    m_finishCallback(status);
  }

  void fail() {
    m_db->updateScanStatus(m_recordId, ScanStatus::Failed);
    m_finishCallback(ScanStatus::Failed);
  }

  void setInterruptFlag() {
    m_interrupted = true;
  }

  void dbAtThread() {
    m_db = std::make_unique<FileIndexerDatabase>();
  }

public:
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

  virtual void interrupt() = 0;

  virtual void join() = 0;
};
