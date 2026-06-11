#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

/**
 * Watches a platform-defined set of "important" directories for structural
 * changes (entries created, deleted or renamed). Each platform implementation
 * owns both the heuristics (which directories matter) and the mechanism used
 * to watch them.
 *
 * This is a best-effort change feed: events may be dropped or coalesced.
 * Periodic incremental scans remain the source of truth for the index.
 */
class ImportantDirectoryWatcher {
public:
  struct Event {
    enum class Kind {
      // the direct contents of `dir` changed
      DirectoryChanged,
      // the watcher lost track of some events (e.g. queue overflow), callers
      // should rescan whatever they care about
      Degraded
    };

    Kind kind;
    std::filesystem::path dir;
  };

  using Callback = std::function<void(const Event &)>;

  virtual ~ImportantDirectoryWatcher() = default;

  virtual std::vector<std::filesystem::path> watchedDirectories() const = 0;

  // The callback is invoked from the watcher's own thread.
  static std::unique_ptr<ImportantDirectoryWatcher> create(Callback cb);
};
