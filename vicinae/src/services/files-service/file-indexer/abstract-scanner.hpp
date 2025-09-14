#pragma once
#include "services/files-service/file-indexer/db-writer.hpp"
#include "services/files-service/file-indexer/scan.hpp"
#include "services/files-service/file-indexer/file-indexer-db.hpp"
#include <QDebug>
#include <memory>

class AbstractScanner {
  /*
   * Runs a scanner in its own thread, calls `finishCallback` when finished.
   *
   * `start()` creates a new record in the DB and sets its status, it should be called in the new thread.
   * `finish()` and `fail()` are internal functions that update the DB and call `finishCallback`.
   * `setInterruptFlag()` makes `finish()` write `Interrupted` instead of `Finished`.
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
  std::shared_ptr<DbWriter> m_writer;

  void start(const Scan &scan) {
    auto result = m_writer->createScan(scan.path, scan.type);

    if (!result.has_value()) {
      qWarning() << "Not scanning" << scan.path.native() << "because scan record creation failed with error"
                 << result.error();
      // TODO: Signal that the scan failed to start
      m_finishCallback(ScanStatus::Failed);
      return;
    }

    m_recordId = result->id;
    m_writer->updateScanStatus(m_recordId, ScanStatus::Started);
  }

  void finish() {
    ScanStatus status = m_interrupted ? ScanStatus::Interrupted : ScanStatus::Succeeded;
    m_writer->updateScanStatus(m_recordId, status);
    m_finishCallback(status);
  }

  void fail() {
    m_writer->updateScanStatus(m_recordId, ScanStatus::Failed);
    m_finishCallback(ScanStatus::Failed);
  }

  void setInterruptFlag() { m_interrupted = true; }

public:
  AbstractScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, FinishCallback callback)
      : m_writer(writer), m_finishCallback(callback) {}
  virtual ~AbstractScanner() = default;

  virtual void interrupt() = 0;

  virtual void join() = 0;
};
