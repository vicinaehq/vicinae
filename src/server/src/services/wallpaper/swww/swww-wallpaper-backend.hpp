#pragma once
#include "services/wallpaper/abstract-wallpaper-backend.hpp"
#include <QString>
#include <optional>

// Drives swww (renamed to awww in 2025) via its CLI: detection is `<bin> query`, which lets
// the binary resolve its own socket path/namespace rather than us guessing the socket name.
class SwwwWallpaperBackend : public AbstractWallpaperBackend {
public:
  std::string id() const override { return "swww"; }
  bool isActivatable() const override;
  bool supportsPerScreen() const override { return true; }
  QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request) override;

private:
  static std::optional<QString> binary();
};
