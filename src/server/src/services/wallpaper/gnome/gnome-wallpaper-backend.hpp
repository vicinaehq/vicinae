#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"

class GnomeWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "gnome"; }
  bool isActivatable() const override;
  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request) override;
};
