#include "mate-wallpaper-backend.hpp"
#include "services/wallpaper/gsettings-fit.hpp"
#include "services/wallpaper/wallpaper-command.hpp"
#include "utils/environment.hpp"
#include <QStandardPaths>
#include <QtConcurrent>

namespace {
constexpr std::string_view SCHEMA = "org.mate.background";
} // namespace

bool MateWallpaperBackend::isActivatable() const {
  return Environment::isMateDesktop() && !QStandardPaths::findExecutable("gsettings").isEmpty();
}

QFuture<std::expected<void, std::string>>
MateWallpaperBackend::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run([request]() -> std::expected<void, std::string> {
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
  });
}
