#include "mate-wallpaper-backend.hpp"
#include "services/wallpaper/gsettings-fit.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include "utils/environment.hpp"
#include <QStandardPaths>

namespace {
constexpr std::string_view SCHEMA = "org.mate.background";
} // namespace

bool MateWallpaperBackend::isActivatable() const {
  return Environment::isMateDesktop() && !QStandardPaths::findExecutable("gsettings").isEmpty();
}

WallpaperCapabilities MateWallpaperBackend::capabilities() const {
  return {
      .perMonitor = false,
      .persistent = true,
      .fitModes = {WallpaperFit::Cover, WallpaperFit::Contain, WallpaperFit::Stretch, WallpaperFit::Center,
                   WallpaperFit::Tile},
  };
}

std::expected<void, std::string> MateWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  const QString path = QString::fromStdString(request.path);
  const QString schema = QString::fromUtf8(SCHEMA.data(), SCHEMA.size());

  if (auto res = wallpaper::runCommand("gsettings", {"set", schema, "picture-filename", path}); !res) {
    return std::unexpected(res.error());
  }

  if (auto res = wallpaper::runCommand(
          "gsettings", {"set", schema, "picture-options", wallpaper::gsettingsPictureOption(request.fit)});
      !res) {
    return std::unexpected(res.error());
  }

  return {};
}
