#pragma once
#include <filesystem>
#include <qobject.h>
#include <qtmetamacros.h>
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

class AbstractFileIndexer : public QObject {
public:
  struct FileResult {
    std::filesystem::path path;
  };

  struct AsyncQuery : public QObject {
    Q_OBJECT

  signals:
  };

  /**
   * An indexing entrypoint.
   */
  struct Entrypoint {
    std::filesystem::path root;
  };

public:
  virtual void start() = 0;
  virtual void setEntrypoints(const std::vector<Entrypoint> &entrypoints) = 0;
  virtual std::vector<FileResult> query(std::string_view view) const = 0;

  virtual ~AbstractFileIndexer() = default;
};
