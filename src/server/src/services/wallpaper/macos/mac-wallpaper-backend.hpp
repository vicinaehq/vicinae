#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

// Sets the wallpaper through AppKit's NSWorkspace, which is main-thread-only, hence the
// synchronous implementation. Only affects the currently active space.
class MacWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "macos"; }
  bool isActivatable() const override { return true; }
  bool supportsPerScreen() const override { return true; }
  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request) override;
};
