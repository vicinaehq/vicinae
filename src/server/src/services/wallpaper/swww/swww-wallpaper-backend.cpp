#include "swww-wallpaper-backend.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include <QStandardPaths>

namespace {

// No tile mode (degrades to crop); "no" means native size, our closest to Center.
QString resizeMode(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
  case WallpaperFit::Tile:
    return "crop";
  case WallpaperFit::Contain:
    return "fit";
  case WallpaperFit::Stretch:
    return "stretch";
  case WallpaperFit::Center:
    return "no";
  }
  return "crop";
}

} // namespace

std::optional<QString> SwwwWallpaperBackend::binary() {
  // awww is the renamed successor of swww; prefer it, fall back to the legacy name.
  for (const auto *name : {"awww", "swww"}) {
    if (auto path = QStandardPaths::findExecutable(name); !path.isEmpty()) { return path; }
  }
  return std::nullopt;
}

bool SwwwWallpaperBackend::isActivatable() const {
  auto bin = binary();
  return bin && wallpaper::runCommand(*bin, {"query"}).has_value();
}

WallpaperCapabilities SwwwWallpaperBackend::capabilities() const {
  return {
      .perMonitor = true,
      .persistent = false,
      .fitModes = {WallpaperFit::Cover, WallpaperFit::Contain, WallpaperFit::Stretch, WallpaperFit::Center},
  };
}

std::expected<void, std::string> SwwwWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  auto bin = binary();
  if (!bin) return std::unexpected("neither awww nor swww is installed");

  QStringList args{"img", "--resize", resizeMode(request.fit)};
  if (request.screen) { args << "--outputs" << QString::fromStdString(*request.screen); }
  args << QString::fromStdString(request.path);

  auto res = wallpaper::runCommand(*bin, args);
  if (!res) return std::unexpected(res.error());

  return {};
}
