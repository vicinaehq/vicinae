#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

class KdeWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "kde"; }
  bool isActivatable() const override;
  std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) override;
};
