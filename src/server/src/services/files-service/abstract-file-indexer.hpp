#pragma once
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

public:
  enum class ScanKind { Full, Incremental, Watcher };
  enum class ScanState { Started, Succeeded, Failed, Interrupted };

  struct ScanStatus {
    int scanId;
    ScanKind kind;
    ScanState state;
    std::filesystem::path entrypoint;
    size_t processedFileCount;

    bool isTerminal() const { return state != ScanState::Started; }
  };

  struct QueryParams {
    int limit;
  };

signals:
  void scanStatusChanged(const AbstractFileIndexer::ScanStatus &status);

public:
  virtual void start() = 0;
  virtual void rebuildIndex() = 0;
  virtual void preferenceValuesChanged(const QJsonObject &preferences) = 0;
  virtual QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                             const QueryParams &params = {}) = 0;

  virtual ~AbstractFileIndexer() = default;
};
