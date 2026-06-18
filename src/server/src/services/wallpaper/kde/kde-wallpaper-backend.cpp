#include "kde-wallpaper-backend.hpp"
#include "utils/dbus.hpp"
#include <QUrl>

namespace {

constexpr const char *PLASMA_SERVICE = "org.kde.plasmashell";
constexpr const char *PLASMA_PATH = "/PlasmaShell";
constexpr const char *PLASMA_IFACE = "org.kde.PlasmaShell";

// FillMode binds to Qt's Image.fillMode enum in the org.kde.image plugin.
int fillMode(WallpaperFit fit) {
  switch (fit) {
  case WallpaperFit::Cover:
    return 2; // PreserveAspectCrop
  case WallpaperFit::Contain:
    return 1; // PreserveAspectFit
  case WallpaperFit::Stretch:
    return 0; // Stretch
  case WallpaperFit::Center:
    return 6; // Pad
  case WallpaperFit::Tile:
    return 3; // Tile
  }
  return 2;
}

} // namespace

bool KdeWallpaperBackend::isActivatable() const { return dbus::isServiceRegistered(PLASMA_SERVICE); }

std::expected<void, std::string> KdeWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  const QString uri = QUrl::fromLocalFile(QString::fromStdString(request.path)).toString();

  const QString script = QString(R"JS(
    var ds = desktops();
    for (var i = 0; i < ds.length; i++) {
      var d = ds[i];
      d.wallpaperPlugin = "org.kde.image";
      d.currentConfigGroup = Array("Wallpaper", "org.kde.image", "General");
      d.writeConfig("Image", "%1");
      d.writeConfig("FillMode", %2);
    }
  )JS")
                             .arg(uri)
                             .arg(fillMode(request.fit));

  auto res = dbus::call(PLASMA_SERVICE, PLASMA_PATH, PLASMA_IFACE, "evaluateScript", {script});
  if (!res) return std::unexpected(res.error().toStdString());

  return {};
}
