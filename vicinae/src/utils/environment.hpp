#pragma once
#include "vicinae.hpp"
#include <QString>
#include <QProcessEnvironment>
#include <qapplication.h>

namespace Environment {

/**
 * Detects if running in GNOME environment
 * Uses same logic as GNOME clipboard server
 */
inline bool isGnomeEnvironment() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  const QString session = qgetenv("GDMSESSION");
  return desktop.contains("GNOME", Qt::CaseInsensitive) || session.contains("gnome", Qt::CaseInsensitive);
}

/**
 * Detects if running on Wayland
 */
inline bool isWaylandSession() { return QApplication::platformName() == "wayland"; }

/**
 * Detects if running in wlroots-based compositor (Hyprland, Sway, etc.)
 */
inline bool isWlrootsCompositor() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  return desktop.contains("Hyprland", Qt::CaseInsensitive) || desktop.contains("sway", Qt::CaseInsensitive) ||
         desktop.contains("river", Qt::CaseInsensitive);
}

inline bool isHudDisabled() { return qEnvironmentVariable("VICINAE_DISABLE_HUD", "0") == "1"; }

inline bool isLayerShellEnabled() { return qEnvironmentVariable("USE_LAYER_SHELL", "1") == "1"; }

inline QStringList fallbackIconSearchPaths() {
  QStringList list;
  auto dirs = Omnicast::xdgDataDirs();

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

  if (isGnomeEnvironment()) {
    desc = "GNOME";
  } else if (isWlrootsCompositor()) {
    desc = "wlroots";
  } else {
    const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
    desc = desktop.isEmpty() ? "Unknown" : desktop;
  }

  if (isWaylandSession()) {
    desc += "/Wayland";
  } else {
    desc += "/X11";
  }

  return desc;
}

} // namespace Environment
