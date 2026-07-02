#pragma once
#include <QFuture>
#include <expected>
#include <optional>
#include <string>

enum class WallpaperFit { Cover, Contain, Stretch, Center, Tile };

struct WallpaperRequest {
  std::string path;
  std::optional<std::string> screen; // best-effort, may be ignored by some backends
  WallpaperFit fit =
      WallpaperFit::Cover; // may degrade to something else if the backend doesn't support a specific fit
};

class AbstractWallpaperBackend {
public:
  virtual ~AbstractWallpaperBackend() = default;

  virtual std::string id() const = 0;

  // whether the backend is _running_ and can accept a request to set the wallpaper
  // e.g if using hyprpaper, this should ensure the hyprpaper daemon is running and ready
  // to accept a IPC call, not only check for the presence of `hyprctl`.
  virtual bool isActivatable() const = 0;

  virtual bool supportsPerScreen() const { return false; }

  virtual QFuture<std::expected<void, std::string>> setWallpaper(const WallpaperRequest &request) = 0;
};
