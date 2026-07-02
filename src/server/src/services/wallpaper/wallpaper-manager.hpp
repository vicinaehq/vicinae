#pragma once
#include "abstract-wallpaper-backend.hpp"
#include <QFuture>
#include <memory>
#include <string>
#include <vector>

namespace config {
class Manager;
}

// Try to set the wallpaper using the currently available backend.
class WallpaperManager {
public:
  explicit WallpaperManager(config::Manager &config) : m_config(config) {}

  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request);

private:
  static std::vector<std::unique_ptr<AbstractWallpaperBackend>>
  createCandidates(const std::string &customCommand);
  static std::unique_ptr<AbstractWallpaperBackend> resolveBackend(const std::string &customCommand);

  std::string customCommand() const;

  config::Manager &m_config;
};
