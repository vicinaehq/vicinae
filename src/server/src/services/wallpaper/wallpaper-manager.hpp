#pragma once
#include "abstract-wallpaper-backend.hpp"
#include <QFuture>
#include <memory>
#include <string>
#include <vector>

namespace config {
class Manager;
}

// A backend is resolved per call (no wallpaper state held), so daemon churn is handled transparently.
// Work runs on a worker thread; config is read on the calling thread and captured into it, so backends
// never touch shared config off-thread.
class WallpaperManager {
public:
  explicit WallpaperManager(config::Manager &config) : m_config(config) {}

  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request);
  WallpaperCapabilities capabilities();

private:
  static std::vector<std::unique_ptr<AbstractWallpaperBackend>>
  createCandidates(const std::string &customCommand);
  static std::unique_ptr<AbstractWallpaperBackend> resolveBackend(const std::string &customCommand);
  static std::expected<void, std::string> apply(const WallpaperRequest &request,
                                                const std::string &customCommand);

  std::string customCommand() const;

  config::Manager &m_config;
};
