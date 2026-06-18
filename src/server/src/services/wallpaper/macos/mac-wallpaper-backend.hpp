#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

// Sets the wallpaper through AppKit's NSWorkspace. The native mechanism is always available on macOS.
class MacWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "macos"; }
  bool isActivatable() const override { return true; }
  bool supportsPerScreen() const override { return true; }
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;
};
