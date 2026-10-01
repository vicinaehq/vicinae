#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

// Sets the wallpaper through the shell's IDesktopWallpaper (Windows 8+).
// On Windows 11, only the current virtual workspace is modified.
// On previous versions of Windows, all workspaces share the same wallpaper.
class WindowsWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "windows"; }
  bool isActivatable() const override { return true; }
  bool supportsPerScreen() const override { return true; }
  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request) override;
};
