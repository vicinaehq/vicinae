#pragma once
#include "abstract-wallpaper-backend.hpp"
#include <QFuture>
#include <memory>
#include <string>
#include <vector>

// Try to set the wallpaper using the currently available backend.
class WallpaperManager {
public:
  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request);

  bool canSetWallpaper();

private:
  static std::vector<std::unique_ptr<AbstractWallpaperBackend>> createCandidates();

  AbstractWallpaperBackend *backend();

  bool m_resolved = false;
  std::unique_ptr<AbstractWallpaperBackend> m_backend;
};
