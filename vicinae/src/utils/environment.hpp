#pragma once
#include "vicinae.hpp"
#include "xdgpp/desktop-entry/iterator.hpp"
#include "xdgpp/env/env.hpp"
#include <QString>
#include <QApplication>
#include <QProcessEnvironment>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <qtenvironmentvariables.h>
#include "version.h"

namespace Environment {

inline bool isGnomeEnvironment() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  const QString session = qgetenv("GDMSESSION");
  return desktop.contains("GNOME", Qt::CaseInsensitive) || session.contains("gnome", Qt::CaseInsensitive);
}

inline bool isWaylandSession() { return QApplication::platformName() == "wayland"; }

inline bool supportsArbitraryWindowPlacement() { return !isWaylandSession(); }

/**
 * Detects if running in wlroots-based compositor (Hyprland, Sway, etc.)
 */
inline bool isWlrootsCompositor() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  return desktop.contains("Hyprland", Qt::CaseInsensitive) || desktop.contains("sway", Qt::CaseInsensitive) ||
         desktop.contains("river", Qt::CaseInsensitive);
}

static inline bool containsIgnoreCase(const std::vector<std::string> &desktops, std::string_view str) {
  return std::ranges::any_of(desktops, [&](auto &desktop) {
    return std::ranges::equal(desktop, str, [](auto &&a, auto &&b) {
      return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
    });
  });
}

inline bool isCosmicDesktop() { return containsIgnoreCase(xdgpp::currentDesktop(), "cosmic"); }
inline bool isPlasmaDesktop() { return containsIgnoreCase(xdgpp::currentDesktop(), "kde"); }
inline bool isGnomeDesktop() { return containsIgnoreCase(xdgpp::currentDesktop(), "gnome"); }

inline bool isLayerShellEnabled() {
#ifndef WAYLAND_LAYER_SHELL
  return false;
#endif
  if (auto value = qEnvironmentVariable("USE_LAYER_SHELL"); !value.isEmpty()) { return value == "1"; }
  return isWaylandSession() && !isCosmicDesktop() && !isGnomeEnvironment();
}

inline bool isHudDisabled() {
  return !isLayerShellEnabled() || qEnvironmentVariable("VICINAE_DISABLE_HUD", "0") == "1";
}

inline bool hasAppLaunchDebug() { return !qEnvironmentVariable("VICINAE_APP_LAUNCH_DEBUG").isEmpty(); }

/**
 * App image directory if we are running in an appimage.
 * We typically use this in order to find the bundled
 * node binary, instead of trying to launch the system one.
 */
inline std::optional<std::filesystem::path> appImageDir() {
  if (auto appdir = getenv("APPDIR")) return appdir;
  return std::nullopt;
}

/**
 * Optional override of the `node` executable to use to spawn the
 * extension manager.
 */
inline std::optional<std::filesystem::path> nodeBinaryOverride() {
  if (auto bin = getenv("VICINAE_NODE_BIN")) return bin;
  return std::nullopt;
}

inline std::chrono::milliseconds pasteDelay() {
  using namespace std::chrono_literals;
  if (const char *delay = getenv("VICINAE_PASTE_DELAY")) {
    return std::chrono::milliseconds(std::stoi(delay));
  }
  return 100ms;
}

inline bool isAppImage() { return appImageDir().has_value(); }

inline int pixmapCacheLimit() {
  static constexpr size_t MB = 1024 * 1024;
  if (const char *delay = getenv("VICINAE_IMAGE_CACHE_SIZE")) { return std::stoi(delay); }
  return 250 * MB; // we will very rarely reach this threshold anyways
}

inline QStringList fallbackIconSearchPaths() {
  QStringList list;
  auto dirs = xdgpp::dataDirs();

  list.reserve(dirs.size() * 2);

  for (const auto &dir : dirs) {
    list << (dir / "pixmaps").c_str();
  }

  for (const auto &dir : dirs) {
    list << (dir / "icons").c_str();
  }

  return list;
}

inline QString vicinaeApiBaseUrl() {
  if (const char *url = getenv("VICINAE_API_BASE_URL")) { return url; }
  return "https://api.vicinae.com/v1";
}

/**
 * Version of the Vicinae app.
 */
inline QString version() { return VICINAE_GIT_TAG; }

/**
 * Gets human-readable environment description
 */
inline QString getEnvironmentDescription() {
  QString desc;
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");

  if (!desktop.isEmpty()) {
    desc = desktop;
  } else if (isGnomeEnvironment()) {
    desc = "GNOME";
  } else if (isWlrootsCompositor()) {
    desc = "wlroots";
  }

  if (isWaylandSession()) {
    desc += "/Wayland";
  } else {
    desc += "/X11";
  }

  return desc;
}

} // namespace Environment
