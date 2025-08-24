#pragma once
#include <QString>
#include <QProcessEnvironment>

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
inline bool isWaylandSession() {
  const QString sessionType = qgetenv("XDG_SESSION_TYPE");
  return sessionType.compare("wayland", Qt::CaseInsensitive) == 0;
}

/**
 * Detects if running in wlroots-based compositor (Hyprland, Sway, etc.)
 */
inline bool isWlrootsCompositor() {
  const QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
  return desktop.contains("Hyprland", Qt::CaseInsensitive) || desktop.contains("sway", Qt::CaseInsensitive) ||
         desktop.contains("river", Qt::CaseInsensitive);
}

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
