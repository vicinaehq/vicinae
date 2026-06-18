#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

class CinnamonWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "cinnamon"; }
  bool isActivatable() const override;
  WallpaperCapabilities capabilities() const override;
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;
};
