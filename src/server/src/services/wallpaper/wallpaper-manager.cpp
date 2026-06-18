#include "wallpaper-manager.hpp"
#include "config/config.hpp"
#include "dummy-wallpaper-backend.hpp"
#include <QtConcurrent>
#include <QtGlobal>
#include <filesystem>
#include "custom/custom-wallpaper-backend.hpp"
#ifdef Q_OS_LINUX
#include "cinnamon/cinnamon-wallpaper-backend.hpp"
#include "gnome/gnome-wallpaper-backend.hpp"
#include "hyprpaper/hyprpaper-wallpaper-backend.hpp"
#include "kde/kde-wallpaper-backend.hpp"
#include "mate/mate-wallpaper-backend.hpp"
#include "swww/swww-wallpaper-backend.hpp"
#endif

std::vector<std::unique_ptr<AbstractWallpaperBackend>>
WallpaperManager::createCandidates(const std::string &customCommand) {
  // XXX - all new wallpaper backends must be added to this vector.
  // A user-configured command overrides everything; then running daemons; then desktop environments.
  std::vector<std::unique_ptr<AbstractWallpaperBackend>> candidates;
  candidates.reserve(7);

  candidates.emplace_back(std::make_unique<CustomCommandWallpaperBackend>(customCommand));

#ifdef Q_OS_LINUX
  candidates.emplace_back(std::make_unique<HyprpaperWallpaperBackend>());
  candidates.emplace_back(std::make_unique<SwwwWallpaperBackend>());
  candidates.emplace_back(std::make_unique<GnomeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<KdeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<CinnamonWallpaperBackend>());
  candidates.emplace_back(std::make_unique<MateWallpaperBackend>());
#endif

  return candidates;
}

std::unique_ptr<AbstractWallpaperBackend> WallpaperManager::resolveBackend(const std::string &customCommand) {
  for (auto &candidate : createCandidates(customCommand)) {
    if (candidate->isActivatable()) { return std::move(candidate); }
  }

  return std::make_unique<DummyWallpaperBackend>();
}

std::expected<void, std::string> WallpaperManager::apply(const WallpaperRequest &request,
                                                         const std::string &customCommand) {
  std::error_code ec;
  if (!std::filesystem::is_regular_file(request.path, ec)) {
    return std::unexpected("No such file: " + request.path);
  }

  return resolveBackend(customCommand)->setWallpaper(request);
}

std::string WallpaperManager::customCommand() const { return m_config.value().wallpaper.command; }

QFuture<std::expected<void, std::string>> WallpaperManager::setWallpaper(const WallpaperRequest &request) {
  return QtConcurrent::run([request, command = customCommand()] { return apply(request, command); });
}

WallpaperCapabilities WallpaperManager::capabilities() {
  return resolveBackend(customCommand())->capabilities();
}
