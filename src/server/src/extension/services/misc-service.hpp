
#include "generated/tsapi.hpp"
#include "services/wallpaper/abstract-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-manager.hpp"
#include <utility>

class MiscService : public tsapi::AbstractMisc {
  Q_OBJECT

  using Void = tsapi::Result<void>;

public:
  MiscService(tsapi::RpcTransport &transport, WallpaperManager &wallpaper)
      : AbstractMisc(transport), m_wallpaper(wallpaper) {}

  tsapi::Result<void>::Future setWallpaper(std::string path, tsapi::SetWallpaperOptions options) override {
    auto req = WallpaperRequest{
        .path = std::move(path),
        .screen = std::move(options.screen),
        .fit = options.fit.transform(mapFit).value_or(WallpaperFit::Cover),
    };

    return m_wallpaper.setWallpaper(req);
  }

private:
  static WallpaperFit mapFit(tsapi::WallpaperFit fit) {
    using AW = tsapi::WallpaperFit;
    switch (fit) {
    case AW::Contain:
      return WallpaperFit::Contain;
    case AW::Center:
      return WallpaperFit::Center;
    case AW::Cover:
      return WallpaperFit::Cover;
    case AW::Stretch:
      return WallpaperFit::Stretch;
    case AW::Tile:
      return WallpaperFit::Tile;
    }
    std::unreachable();
  }

  WallpaperManager &m_wallpaper;
};
