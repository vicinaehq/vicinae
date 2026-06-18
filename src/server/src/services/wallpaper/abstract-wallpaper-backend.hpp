#pragma once
#include <expected>
#include <optional>
#include <set>
#include <string>

enum class WallpaperFit { Cover, Contain, Stretch, Center, Tile };

struct WallpaperCapabilities {
  bool perMonitor = false;
  bool persistent = false; // survives a session restart
  std::set<WallpaperFit> fitModes;
};

struct WallpaperRequest {
  std::string path;
  std::optional<std::string> screen; // best-effort; ignored by single-canvas backends
  WallpaperFit fit = WallpaperFit::Cover;
};

class AbstractWallpaperBackend {
public:
  virtual ~AbstractWallpaperBackend() = default;

  virtual std::string id() const = 0;

  // Must reflect live state (e.g. a running daemon socket), not merely whether a tool is installed.
  virtual bool isActivatable() const = 0;

  virtual WallpaperCapabilities capabilities() const = 0;

  virtual std::expected<void, std::string> setWallpaper(const WallpaperRequest &request) = 0;
};
