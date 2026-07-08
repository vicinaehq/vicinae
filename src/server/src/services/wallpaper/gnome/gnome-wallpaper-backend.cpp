#include "gnome-wallpaper-backend.hpp"
#include "services/wallpaper/gsettings-fit.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include "utils/environment.hpp"
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>

namespace {

constexpr std::string_view SCHEMA = "org.gnome.desktop.background";

} // namespace

bool GnomeWallpaperBackend::isActivatable() const {
  return Environment::isGnomeEnvironment() && !QStandardPaths::findExecutable("gsettings").isEmpty();
}

QFuture<std::expected<void, std::string>>
GnomeWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run([request]() -> std::expected<void, std::string> {
    const QString uri = QUrl::fromLocalFile(QString::fromStdString(request.path)).toString();
    const QString schema = QString::fromUtf8(SCHEMA.data(), SCHEMA.size());

    if (auto res = wallpaper::runCommand("gsettings", {"set", schema, "picture-uri", uri}); !res) {
      return std::unexpected(res.error());
    }

    // picture-uri-dark only exists since GNOME 42; ignore failure on older versions.
    (void)wallpaper::runCommand("gsettings", {"set", schema, "picture-uri-dark", uri});

    auto res = wallpaper::runCommand(
        "gsettings", {"set", schema, "picture-options", wallpaper::gsettingsPictureOption(request.fit)});
    if (!res) return std::unexpected(res.error());

    return {};
  });
}
