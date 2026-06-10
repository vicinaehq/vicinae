#pragma once
#include "file-indexer/db-writer.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/log.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>

class AbstractScanner {
  /*
   * Runs a scanner in its own thread, calls `statusCallback` with `Started` when the scan
   * begins and for throttled progress ticks, then exactly once with a terminal status.
   *
   * `start()` creates a new record in the DB and sets its status, it should be called in the new thread.
   * `finish()` and `fail()` are internal functions that update the DB and call `statusCallback`.
   * `setInterruptFlag()` makes `finish()` write `Interrupted` instead of `Finished`.
   *
   * `interrupt()` signals the scanner to stop prematurely.
   * `join()` joins any threads and sets the scanner up for deconstruction.
   * This waits until the scan is finished - call `interrupt()` to stop faster.
   */

public:
  using StatusCallback = std::function<void(ScanStatus, size_t processedCount)>;

protected:
  std::shared_ptr<DbWriter> m_writer;

private:
  static constexpr std::chrono::milliseconds PROGRESS_NOTIFY_INTERVAL{500};

  StatusCallback m_statusCallback;
  int m_recordId;
  size_t m_processedCount = 0;
  std::chrono::steady_clock::time_point m_lastProgressNotify;
  std::atomic<bool> m_interrupted = false;

protected:
  void start(const Scan &scan) {
    auto result = m_writer->createScan(scan.path, scan.type);

    if (!result.has_value()) {
      flog::warn() << "Not scanning" << scan.path.native() << "because scan record creation failed with error"
                   << result.error();
      // TODO: Signal that the scan failed to start
      m_statusCallback(ScanStatus::Failed, m_processedCount);
      return;
    }

    m_recordId = result->id;
    m_writer->updateScanStatus(m_recordId, ScanStatus::Started);
    m_lastProgressNotify = std::chrono::steady_clock::now();
    m_statusCallback(ScanStatus::Started, m_processedCount);
  }

  void reportProgress(size_t count = 1) {
    m_processedCount += count;

    auto const now = std::chrono::steady_clock::now();
    if (now - m_lastProgressNotify < PROGRESS_NOTIFY_INTERVAL) return;

    m_lastProgressNotify = now;
    m_statusCallback(ScanStatus::Started, m_processedCount);
  }

  void finish() {
    ScanStatus status = m_interrupted ? ScanStatus::Interrupted : ScanStatus::Succeeded;
    m_writer->updateScanStatus(m_recordId, status);
    m_statusCallback(status, m_processedCount);
  }

  void fail() {
    m_writer->updateScanStatus(m_recordId, ScanStatus::Failed);
    m_statusCallback(ScanStatus::Failed, m_processedCount);
  }

  void setInterruptFlag() { m_interrupted = true; }

public:
  AbstractScanner(std::shared_ptr<DbWriter> writer, const Scan &scan, StatusCallback callback)
      : m_writer(writer), m_statusCallback(callback) {}
  virtual ~AbstractScanner() = default;

  virtual void interrupt() = 0;

  virtual void join() = 0;
};
