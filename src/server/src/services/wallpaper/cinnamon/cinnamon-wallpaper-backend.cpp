#include "cinnamon-wallpaper-backend.hpp"
#include "services/wallpaper/gsettings-fit.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include "utils/environment.hpp"
#include <QStandardPaths>
#include <QUrl>
#include <QtConcurrent>

namespace {
constexpr std::string_view SCHEMA = "org.cinnamon.desktop.background";
} // namespace

bool CinnamonWallpaperBackend::isActivatable() const {
  return Environment::isCinnamonDesktop() && !QStandardPaths::findExecutable("gsettings").isEmpty();
}

QFuture<std::expected<void, std::string>>
CinnamonWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run([request]() -> std::expected<void, std::string> {
    const QString uri = QUrl::fromLocalFile(QString::fromStdString(request.path)).toString();
    const QString schema = QString::fromUtf8(SCHEMA.data(), SCHEMA.size());

    if (auto res = wallpaper::runCommand("gsettings", {"set", schema, "picture-uri", uri}); !res) {
      return std::unexpected(res.error());
    }

    if (auto res = wallpaper::runCommand(
            "gsettings", {"set", schema, "picture-options", wallpaper::gsettingsPictureOption(request.fit)});
        !res) {
      return std::unexpected(res.error());
    }

    return {};
  });
}
