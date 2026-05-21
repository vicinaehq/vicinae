#pragma once
#include <cstdint>
#include <qfuture.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <vector>
#include <filesystem>

/**
 * A file indexer capable of indexing a LOT of files (technically, a whole filesystem, if the user wants so)
 *
 * On MacOS, the indexer essentially calls into the Spotlight API and does not handle any indexing by itself.
 *
 * On Windows, no specialized API seem to be available.
 *
 * On Linux, there is no standard desktop environment (DE) agnostic way to handle file indexing. So we can:
 * - Detect the DE in use and use a specialized file indexer that calls into the DE search API (assuming it's
 * exposed)
 * - Provide an in-house file indexing solution for environments without a file indexing solution.
 *
 * For the time being we will focus on the in-house solution that will successfully cover all Linux
 * environments.
 */

struct IndexerFileResult {
  std::filesystem::path path;
  double rank;
};

struct IndexerAsyncQuery : public QObject {
  Q_OBJECT

signals:
  void finished(const std::vector<IndexerFileResult> &results) const;
};

struct Pagination {
  int offset = 0;
  int limit = 50;
};

class AbstractFileIndexer : public QObject {
  Q_OBJECT

signals:
  /** Emitted when underlying indexer changes the amount of scanned files */
  void scanStateChanged(quint64 scannedFilesCount, bool isScanning, bool isReady);

public:
  struct ScanState {
    std::uint64_t scannedFilesCount = 0;
    bool isScanning = false;
    bool isReady = false;
  };

  struct QueryParams {
    Pagination pagination;
  };

public:
  virtual void start() = 0;

  /** Clears all the in-memory file indexer */
  virtual void stop() {}

  virtual void rebuildIndex() = 0;
  virtual void preferenceValuesChanged(const QJsonObject &preferences) = 0;
  virtual QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                             const QueryParams &params = {}) = 0;

  /**
   * Snapshot of the current scan state. Thread-safe; may briefly contend on an internal mutex.
   */
  virtual ScanState scanState() const = 0;

  ~AbstractFileIndexer() override = default;
};
