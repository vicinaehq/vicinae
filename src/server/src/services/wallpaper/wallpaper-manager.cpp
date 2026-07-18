#include "wallpaper-manager.hpp"
#include <QtGlobal>
#include <filesystem>
#ifdef Q_OS_LINUX
#include "cinnamon/cinnamon-wallpaper-backend.hpp"
#include "gnome/gnome-wallpaper-backend.hpp"
#include "hyprpaper/hyprpaper-wallpaper-backend.hpp"
#include "kde/kde-wallpaper-backend.hpp"
#include "mate/mate-wallpaper-backend.hpp"
#include "swww/swww-wallpaper-backend.hpp"
#endif
#ifdef Q_OS_MACOS
#include "macos/mac-wallpaper-backend.hpp"
#endif
#ifdef Q_OS_WIN
#include "windows/windows-wallpaper-backend.hpp"
#endif

std::vector<std::unique_ptr<AbstractWallpaperBackend>> WallpaperManager::createCandidates() {
  // XXX - all new wallpaper backends must be added to this vector.
  // Running daemons take precedence over desktop environments.
  std::vector<std::unique_ptr<AbstractWallpaperBackend>> candidates;
  candidates.reserve(7);

#ifdef Q_OS_LINUX
  candidates.emplace_back(std::make_unique<HyprpaperWallpaperBackend>());
  candidates.emplace_back(std::make_unique<SwwwWallpaperBackend>());
  candidates.emplace_back(std::make_unique<GnomeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<KdeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<CinnamonWallpaperBackend>());
  candidates.emplace_back(std::make_unique<MateWallpaperBackend>());
#endif
#ifdef Q_OS_MACOS
  candidates.emplace_back(std::make_unique<MacWallpaperBackend>());
#endif
#ifdef Q_OS_WIN
  candidates.emplace_back(std::make_unique<WindowsWallpaperBackend>());
#endif

  return candidates;
}

AbstractWallpaperBackend *WallpaperManager::backend() {
  if (!m_resolved) {
    for (auto &candidate : createCandidates()) {
      if (candidate->isActivatable()) {
        m_backend = std::move(candidate);
        break;
      }
    }
    m_resolved = true;
  }

  return m_backend.get();
}

bool WallpaperManager::canSetWallpaper() { return backend() != nullptr; }

QFuture<std::expected<void, std::string>> WallpaperManager::setWallpaper(const WallpaperRequest &request) {
  auto makeError = [](std::string message) {
    return QtFuture::makeReadyValueFuture<std::expected<void, std::string>>(
        std::unexpected(std::move(message)));
  };

  auto *backend = this->backend();

  if (!backend) return makeError("Setting the wallpaper is not supported in the current environment");

  std::error_code ec;
  if (!std::filesystem::is_regular_file(request.path, ec)) {
    return makeError("No such file: " + request.path);
  }

  return backend->setWallpaper(request);
}
