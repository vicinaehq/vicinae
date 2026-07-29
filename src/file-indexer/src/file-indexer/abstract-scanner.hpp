#pragma once
#include "file-indexer/db-writer.hpp"
#include "file-indexer/scan.hpp"
#include "file-indexer/file-indexer-db.hpp"
#include "file-indexer/log.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <utility>

class AbstractScanner {
public:
  using StatusCallback = std::function<void(ScanStatus, size_t processedCount)>;

protected:
  std::shared_ptr<DbWriter> m_writer;
  Scan m_scan;

private:
  static constexpr std::chrono::milliseconds PROGRESS_NOTIFY_INTERVAL{500};

  StatusCallback m_statusCallback;
  int m_recordId = -1;
  size_t m_processedCount = 0;
  std::chrono::steady_clock::time_point m_lastProgressNotify;
  std::atomic<bool> m_interrupted = false;

protected:
  void start() {
    auto result = m_writer->createScan(m_scan.path, m_scan.type());

    if (!result.has_value()) {
      flog::warn() << "Not scanning" << m_scan.path.native()
                   << "because scan record creation failed with error" << result.error();
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
    if (m_recordId >= 0) {
      m_writer->finalizeScan(m_recordId, status, static_cast<int64_t>(m_processedCount));
    }
    m_statusCallback(status, m_processedCount);
  }

  void fail() {
    if (m_recordId >= 0) {
      m_writer->finalizeScan(m_recordId, ScanStatus::Failed, static_cast<int64_t>(m_processedCount));
    }
    m_statusCallback(ScanStatus::Failed, m_processedCount);
  }

  void setInterruptFlag() { m_interrupted = true; }

  bool isInterrupted() const { return m_interrupted; }

public:
  AbstractScanner(std::shared_ptr<DbWriter> writer, Scan scan, StatusCallback callback)
      : m_writer(std::move(writer)), m_scan(std::move(scan)), m_statusCallback(std::move(callback)) {}
  virtual ~AbstractScanner() = default;

  virtual void run() = 0;
  virtual void interrupt() = 0;
};
