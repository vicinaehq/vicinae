
#include "generated/tsapi.hpp"
#include "services/wallpaper/abstract-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-manager.hpp"
#include <utility>

class MiscService : public tsapi::AbstractMisc {
  Q_OBJECT

  using Void = tsapi::Result<void>;

public:
  MiscService(tsapi::RpcTransport &transport) : AbstractMisc(transport) {}

  tsapi::Result<void>::Future setWallpaper(std::string path, tsapi::SetWallpaperOptions options) override {
    auto req = WallpaperRequest{
        .path = std::move(path),
        .screen = std::move(options.screen),
        .fit = options.fit.transform(mapFit).value_or(WallpaperFit::Cover),
    };

    return WallpaperManager::setWallpaper(req);
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
    case AW::Title:
      return WallpaperFit::Tile;
    }
    std::unreachable();
  }
};
