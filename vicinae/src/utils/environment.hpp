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

inline bool isCosmicDesktop() {
  auto dd = xdgpp::currentDesktop();
  return std::ranges::find(dd, "COSMIC") != dd.end();
}

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
  if (auto delay = getenv("VICINAE_PASTE_DELAY")) { return std::chrono::milliseconds(std::stoi(delay)); }
  return 100ms;
}

inline bool isAppImage() { return appImageDir().has_value(); }

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
