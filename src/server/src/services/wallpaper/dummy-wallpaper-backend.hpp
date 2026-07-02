#pragma once
#include "abstract-wallpaper-backend.hpp"

class DummyWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "dummy"; }
  bool isActivatable() const override { return false; }

  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &) override {
    return QtFuture::makeReadyValueFuture<std::expected<void, std::string>>(
        std::unexpected("Setting the wallpaper is not supported in the current environment"));
  }
};
