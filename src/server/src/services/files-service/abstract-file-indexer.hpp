#pragma once
#include <common/file-category.hpp>
#include <qfuture.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

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
  vicinae::FileCategory category = vicinae::FileCategory::Other;
  std::optional<std::string> mimeType;
};

struct IndexerQueryParams {
  int limit = 100;
  std::optional<vicinae::FileCategory> category;
};

struct IndexerAsyncQuery : public QObject {
  Q_OBJECT

signals:
  void finished(const std::vector<IndexerFileResult> &results) const;
};

class AbstractFileIndexer : public QObject {
  Q_OBJECT

public:
  enum class ScanKind { Full, Incremental };
  enum class ScanState { Started, Succeeded, Failed, Interrupted };

  struct ScanStatus {
    int scanId;
    ScanKind kind;
    ScanState state;
    std::filesystem::path entrypoint;
    size_t processedFileCount;

    bool isTerminal() const { return state != ScanState::Started; }
  };

signals:
  void scanStatusChanged(const AbstractFileIndexer::ScanStatus &status);

public:
  virtual void start() = 0;
  virtual void rebuildIndex() = 0;
  virtual void preferenceValuesChanged(const QJsonObject &preferences) = 0;
  virtual QFuture<std::vector<IndexerFileResult>> queryAsync(std::string_view view,
                                                             const IndexerQueryParams &params = {}) = 0;

  virtual ~AbstractFileIndexer() = default;
};
