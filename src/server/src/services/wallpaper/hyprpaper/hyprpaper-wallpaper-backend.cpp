#include "hyprpaper-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include <QStandardPaths>
#include <QtConcurrent>

namespace {

// hyprpaper supports a per-wallpaper mode prefix; cover is the implicit default.
QString modePrefix(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Contain:
    return "contain";
  case WallpaperFit::Tile:
    return "tile";
  case WallpaperFit::Cover:
    return "cover";
  case WallpaperFit::Stretch:
    return "fill";
  case WallpaperFit::Center:
    return "";
  }
  return "";
}

} // namespace

bool HyprpaperWallpaperBackend::isActivatable() const {
  return !QStandardPaths::findExecutable("hyprctl").isEmpty() &&
         wallpaper::runCommand("hyprctl", {"hyprpaper", "listactive"}).has_value();
}

QFuture<std::expected<void, std::string>>
HyprpaperWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run([request]() -> std::expected<void, std::string> {
    const QString path = QString::fromStdString(request.path);

    const QString monitor = request.screen ? QString::fromStdString(*request.screen) : QString{};
    const QString spec = QString{"%1,%2,%3"}.arg(monitor, path, modePrefix(request.fit));

    if (auto res = wallpaper::runCommand("hyprctl", {"hyprpaper", "wallpaper", spec}); !res) {
      return std::unexpected(res.error());
    }

    return {};
  });
}
