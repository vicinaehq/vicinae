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
  for (const auto *name : {"awww", "swww"}) {
    if (auto path = QStandardPaths::findExecutable(name); !path.isEmpty()) { return path; }
  }
  return std::nullopt;
}

bool SwwwWallpaperBackend::isActivatable() const {
  auto bin = binary();
  return bin && wallpaper::runCommand(*bin, {"query"}).has_value();
}

std::expected<void, std::string> SwwwWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  auto bin = binary();
  if (!bin) return std::unexpected("neither awww nor swww is installed");

  // swww defaults to a fade transition; set instantly to match the other backends.
  QStringList args{"img", "--transition-type", "none", "--resize", resizeMode(request.fit)};
  if (request.screen) { args << "--outputs" << QString::fromStdString(*request.screen); }
  args << QString::fromStdString(request.path);

  auto res = wallpaper::runCommand(*bin, args);
  if (!res) return std::unexpected(res.error());

  return {};
}
