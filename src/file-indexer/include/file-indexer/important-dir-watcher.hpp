#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

/**
 * Best-effort structural change feed over a platform-defined set of important
 * directories: events may be dropped or coalesced, periodic scans remain the
 * source of truth. Each platform owns both heuristics and mechanism.
 */
class ImportantDirectoryWatcher {
public:
  struct Event {
    enum class Kind { DirectoryChanged, Degraded };

    Kind kind;
    std::filesystem::path dir;
  };

  using Callback = std::function<void(const Event &)>;

  virtual ~ImportantDirectoryWatcher() = default;

  // the heuristic root set: what periodic sweeps should cover
  virtual std::vector<std::filesystem::path> rootDirectories() const = 0;

  // every currently watched directory (roots + bounded-depth descendants)
  virtual std::vector<std::filesystem::path> watchedDirectories() const = 0;

  // replaces the dynamic watch tier; already-watched paths are ignored, absent
  // ones are unwatched. Shares the budget with the static set.
  virtual void setDynamicDirectories(const std::vector<std::filesystem::path> &dirs) = 0;

  // The callback is invoked from the watcher's own thread.
  static std::unique_ptr<ImportantDirectoryWatcher> create(Callback cb);
};
