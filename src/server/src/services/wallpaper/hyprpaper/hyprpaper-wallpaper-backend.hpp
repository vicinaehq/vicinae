#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

// Driven through `hyprctl hyprpaper`, so detection goes through hyprctl too: no hyprctl means
// we can't talk to hyprpaper anyway, and `listloaded` confirms the daemon is actually responding.
class HyprpaperWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "hyprpaper"; }
  bool isActivatable() const override;
  WallpaperCapabilities capabilities() const override;
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;
};
