#pragma once
#include "abstract-wallpaper-backend.hpp"
#include <QFuture>
#include <filesystem>
#include <memory>
#include <optional>
#include <vector>

// Resolves a backend on demand for every call; there is no long-running service, so
// daemons starting, stopping or being swapped at runtime are handled transparently.
//
// The backends block on a child process or DBus reply, so setWallpaper/reapplyPersisted
// run the work on a worker thread and hand back a future.
class WallpaperManager {
public:
  static std::unique_ptr<AbstractWallpaperBackend> resolveBackend();

  // When persist is true, a successful apply is also stored for reapplyPersisted().
  static QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request,
                                                                bool persist = false);

  // Re-apply the persisted wallpaper at startup so non-persistent backends (swww,
  // hyprpaper) survive a session restart. Unconditional: persistence is opt-in.
  static QFuture<std::expected<void, std::string>> reapplyPersisted();

  static WallpaperCapabilities capabilities();

private:
  static std::vector<std::unique_ptr<AbstractWallpaperBackend>> createCandidates();
  static std::expected<void, std::string> apply(const WallpaperRequest &request, bool persist);
  static std::filesystem::path persistedStatePath();
  static void writeState(const WallpaperRequest &request);
  static std::optional<WallpaperRequest> readState();
};
