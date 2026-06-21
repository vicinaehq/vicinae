#pragma once
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>

class ImportantDirectoryWatcher {
public:
  struct Event {
    enum class Kind { DirectoryChanged, Degraded };

    Kind kind;
    std::filesystem::path dir;
  };

  using Callback = std::function<void(const Event &)>;

  virtual ~ImportantDirectoryWatcher() = default;

  virtual std::vector<std::filesystem::path> rootDirectories() const = 0;

  virtual std::vector<std::filesystem::path> watchedDirectories() const = 0;

  virtual void setDynamicDirectories(const std::vector<std::filesystem::path> &dirs) = 0;

  static std::unique_ptr<ImportantDirectoryWatcher> create(Callback cb);
};
