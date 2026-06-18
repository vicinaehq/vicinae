#include "hyprpaper-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include <QStandardPaths>

namespace {

// hyprpaper supports a per-wallpaper mode prefix; cover is the implicit default.
QString modePrefix(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Contain:
    return "contain:";
  case WallpaperFit::Tile:
    return "tile:";
  case WallpaperFit::Cover:
  case WallpaperFit::Stretch:
  case WallpaperFit::Center:
    return "";
  }
  return "";
}

} // namespace

bool HyprpaperWallpaperBackend::isActivatable() const {
  return !QStandardPaths::findExecutable("hyprctl").isEmpty() &&
         wallpaper::runCommand("hyprctl", {"hyprpaper", "listloaded"}).has_value();
}

WallpaperCapabilities HyprpaperWallpaperBackend::capabilities() const {
  return {
      .perMonitor = true,
      .persistent = false,
      .fitModes = {WallpaperFit::Cover, WallpaperFit::Contain, WallpaperFit::Tile},
  };
}

std::expected<void, std::string> HyprpaperWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  const QString path = QString::fromStdString(request.path);

  if (auto preload = wallpaper::runCommand("hyprctl", {"hyprpaper", "preload", path}); !preload) {
    return std::unexpected(preload.error());
  }

  const QString monitor = request.screen ? QString::fromStdString(*request.screen) : QString{};
  const QString spec = QString{"%1,%2%3"}.arg(monitor, modePrefix(request.fit), path);

  if (auto res = wallpaper::runCommand("hyprctl", {"hyprpaper", "wallpaper", spec}); !res) {
    return std::unexpected(res.error());
  }

  return {};
}
