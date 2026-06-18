#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

class MateWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "mate"; }
  bool isActivatable() const override;
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;
};
