#include "wallpaper-manager.hpp"
#include "dummy-wallpaper-backend.hpp"
#include "vicinae.hpp"
#include <QtConcurrent>
#include <QtGlobal>
#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <qlogging.h>
#ifdef Q_OS_LINUX
#include "cinnamon/cinnamon-wallpaper-backend.hpp"
#include "gnome/gnome-wallpaper-backend.hpp"
#include "hyprpaper/hyprpaper-wallpaper-backend.hpp"
#include "kde/kde-wallpaper-backend.hpp"
#include "mate/mate-wallpaper-backend.hpp"
#include "swww/swww-wallpaper-backend.hpp"
#endif

template <> struct glz::meta<WallpaperFit> {
  using enum WallpaperFit;
  static constexpr auto value =
      glz::enumerate("cover", Cover, "contain", Contain, "stretch", Stretch, "center", Center, "tile", Tile);
};

namespace {
constexpr const char *STATE_FILE = "wallpaper.json";
} // namespace

std::vector<std::unique_ptr<AbstractWallpaperBackend>> WallpaperManager::createCandidates() {
  // XXX - all new wallpaper backends must be added to this vector.
  // Running-daemon backends come first, desktop-environment backends are the fallback.
  std::vector<std::unique_ptr<AbstractWallpaperBackend>> candidates;

#ifdef Q_OS_LINUX
  candidates.reserve(6);
  candidates.emplace_back(std::make_unique<HyprpaperWallpaperBackend>());
  candidates.emplace_back(std::make_unique<SwwwWallpaperBackend>());
  candidates.emplace_back(std::make_unique<GnomeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<KdeWallpaperBackend>());
  candidates.emplace_back(std::make_unique<CinnamonWallpaperBackend>());
  candidates.emplace_back(std::make_unique<MateWallpaperBackend>());
#endif

  return candidates;
}

std::unique_ptr<AbstractWallpaperBackend> WallpaperManager::resolveBackend() {
  for (auto &candidate : createCandidates()) {
    if (candidate->isActivatable()) { return std::move(candidate); }
  }

  return std::make_unique<DummyWallpaperBackend>();
}

std::expected<void, std::string> WallpaperManager::apply(const WallpaperRequest &request, bool persist) {
  std::error_code ec;
  if (!std::filesystem::is_regular_file(request.path, ec)) {
    return std::unexpected("No such file: " + request.path);
  }

  auto res = resolveBackend()->setWallpaper(request);
  if (res && persist) { writeState(request); }

  return res;
}

QFuture<std::expected<void, std::string>> WallpaperManager::setWallpaper(const WallpaperRequest &request,
                                                                         bool persist) {
  return QtConcurrent::run([request, persist] { return apply(request, persist); });
}

QFuture<std::expected<void, std::string>> WallpaperManager::reapplyPersisted() {
  return QtConcurrent::run([] {
    auto state = readState();
    if (!state) return std::expected<void, std::string>{};

    auto res = apply(*state, false);
    if (!res) { qWarning() << "Failed to re-apply persisted wallpaper:" << res.error().c_str(); }

    return res;
  });
}

WallpaperCapabilities WallpaperManager::capabilities() { return resolveBackend()->capabilities(); }

std::filesystem::path WallpaperManager::persistedStatePath() { return Omnicast::stateDir() / STATE_FILE; }

void WallpaperManager::writeState(const WallpaperRequest &request) {
  std::string buf;
  if (const auto error = glz::write_file_json(request, persistedStatePath().string(), buf)) {
    qWarning() << "Failed to persist wallpaper state:" << glz::format_error(error).c_str();
  }
}

std::optional<WallpaperRequest> WallpaperManager::readState() {
  const auto path = persistedStatePath();
  std::error_code ec;
  if (!std::filesystem::exists(path, ec)) return std::nullopt;

  WallpaperRequest request;
  std::string buf;
  if (const auto error = glz::read_file_json(request, path.string(), buf)) {
    qWarning() << "Failed to read persisted wallpaper state:" << glz::format_error(error).c_str();
    return std::nullopt;
  }

  return request;
}
