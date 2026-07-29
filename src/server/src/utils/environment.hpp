#pragma once
#include <QtGlobal>
#ifndef Q_OS_WIN
#include "xdgpp/env/env.hpp"
#endif
#include <QString>
#include <QGuiApplication>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <qtenvironmentvariables.h>

#ifdef Q_OS_LINUX
#include "internal/wayland/globals.hpp"
#endif

namespace Environment {

#ifdef Q_OS_WIN
inline std::vector<std::string> platformDesktopNames() { return {}; }
inline std::vector<std::filesystem::path> platformDataDirs() { return {}; }
#else
inline auto platformDesktopNames() { return xdgpp::currentDesktop(); }
inline auto platformDataDirs() { return xdgpp::dataDirs(); }
#endif

inline bool isGnomeEnvironment() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  const QString session = qgetenv("GDMSESSION");
  return desktop.contains("GNOME", Qt::CaseInsensitive) || session.contains("gnome", Qt::CaseInsensitive);
}

inline bool isWaylandSession() { return QGuiApplication::platformName() == "wayland"; }

inline bool isX11() { return QGuiApplication::platformName() == "xcb"; }

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

inline bool isCosmicDesktop() { return containsIgnoreCase(platformDesktopNames(), "cosmic"); }
inline bool isNiriCompositor() { return containsIgnoreCase(platformDesktopNames(), "niri"); }
inline bool isHyprlandCompositor() { return containsIgnoreCase(platformDesktopNames(), "Hyprland"); }
inline bool isPlasmaDesktop() { return containsIgnoreCase(platformDesktopNames(), "kde"); }
inline bool isWaylandPlasmaDesktop() { return isWaylandSession() && isPlasmaDesktop(); }
inline bool isGnomeDesktop() { return containsIgnoreCase(platformDesktopNames(), "gnome"); }
inline bool isCinnamonDesktop() {
  // "X-Cinnamon" is the pre-spec value (used by Linux Mint); "Cinnamon" is the registered one.
  return containsIgnoreCase(platformDesktopNames(), "x-cinnamon") ||
         containsIgnoreCase(platformDesktopNames(), "cinnamon");
}
inline bool isMateDesktop() { return containsIgnoreCase(platformDesktopNames(), "mate"); }
inline bool isXfceDesktop() { return containsIgnoreCase(platformDesktopNames(), "xfce"); }

// used mostly to exclude cosmic which's implementation is currently broken
inline bool isLayerShellSupported() {
#ifndef WAYLAND_LAYER_SHELL
  return false;
#elifdef Q_OS_LINUX
  return Wayland::Globals::layerShell() && !isCosmicDesktop();
#else
  return false;
#endif
}

inline bool isHudSupported() {
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
  return true;
#else
  // if layer shell is not supported, there doesn't seem to be an easy way for us to create
  // a window that doesn't steal focus.
  return isLayerShellSupported();
#endif
}

inline bool isHudDisabled() { return !isHudSupported(); }

/**
 * Optional override of the `node` executable to use to spawn the
 * extension manager.
 */
inline std::optional<std::filesystem::path> nodeBinaryOverride() {
  if (auto bin = getenv("VICINAE_NODE_BIN")) return bin;
  return std::nullopt;
}

inline QStringList fallbackIconSearchPaths() {
  QStringList list;
  auto dirs = platformDataDirs();

  list.reserve(dirs.size() * 2);

  for (const auto &dir : dirs) {
    list << QString::fromStdString((dir / "pixmaps").string());
  }

  for (const auto &dir : dirs) {
    list << QString::fromStdString((dir / "icons").string());
  }

  return list;
}

inline QString vicinaeApiBaseUrl() {
  if (const char *url = getenv("VICINAE_API_URL")) { return url; }
  return "https://api.vicinae.com/v1";
}

inline QString updateFeedUrl() {
  if (const char *url = getenv("VICINAE_UPDATE_FEED_URL")) { return url; }
  return "https://api.github.com/repos/vicinaehq/vicinae/releases/latest";
}

inline std::optional<std::string> updateVersionOverride() {
  if (const char *version = getenv("VICINAE_UPDATE_FORCE_VERSION")) { return version; }
  return std::nullopt;
}

/**
 * Gets human-readable environment description
 */
inline QString getEnvironmentDescription() {
#ifdef Q_OS_MACOS
  return QStringLiteral("Aqua");
#else
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
#endif
}

inline std::string chassisType() {
  std::ifstream file("/sys/class/dmi/id/chassis_type");
  if (!file.is_open()) return "unknown";

  int type = 0;
  file >> type;

  switch (type) {
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
    return "desktop";
  case 8:
  case 9:
  case 10:
  case 14:
  case 11:
  case 12:
  case 13:
  case 30:
  case 31:
  case 32:
    return "laptop";
  default:
    return "other";
  }
}

inline std::optional<QString> detectAppLauncher() {
  QProcess proc;
  proc.start("uwsm", {"check", "is-active"});
  if (!proc.waitForFinished(1000) || proc.exitCode() != 0) return std::nullopt;
  if (!QStandardPaths::findExecutable("uwsm-app").isEmpty()) return "uwsm-app --";
  return "uwsm app --";
}

} // namespace Environment
