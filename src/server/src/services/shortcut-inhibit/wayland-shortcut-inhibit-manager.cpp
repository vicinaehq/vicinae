#include "wayland-shortcut-inhibit-manager.hpp"
#include <QGuiApplication>
#include <qevent.h>
#include <qlogging.h>
#include "qt-wayland-utils.hpp"

WaylandShortcutInhibitManager::WaylandShortcutInhibitManager(
    zwp_keyboard_shortcuts_inhibit_manager_v1 *manager)
    : m_manager(manager) {}

WaylandShortcutInhibitManager::~WaylandShortcutInhibitManager() {
  for (auto &[win, inhibitor] : m_inhibitors) {
    zwp_keyboard_shortcuts_inhibitor_v1_destroy(inhibitor);
  }
}

bool WaylandShortcutInhibitManager::inhibit(QWindow *win) {
  if (m_inhibitors.contains(win)) { return true; }

  auto *surface = QtWaylandUtils::getWindowSurface(win);

  if (!surface) {
    qWarning() << "Failed to get wl_surface for window" << win;
    return false;
  }

  auto *seat = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->seat();
  auto *inhibitor = zwp_keyboard_shortcuts_inhibit_manager_v1_inhibit_shortcuts(m_manager, surface, seat);

  if (!inhibitor) { return false; }

  win->installEventFilter(this);
  m_inhibitors[win] = inhibitor;

  return true;
}

bool WaylandShortcutInhibitManager::release(QWindow *win) {
  auto it = m_inhibitors.find(win);

  if (it == m_inhibitors.end()) { return false; }

  win->removeEventFilter(this);
  zwp_keyboard_shortcuts_inhibitor_v1_destroy(it->second);
  m_inhibitors.erase(it);

  return true;
}

bool WaylandShortcutInhibitManager::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::PlatformSurface) {
    auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT

    if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      if (auto it = m_inhibitors.find(qobject_cast<QWindow *>(sender)); it != m_inhibitors.end()) {
        zwp_keyboard_shortcuts_inhibitor_v1_destroy(it->second);
        m_inhibitors.erase(it);
      }
    }
  }

  return QObject::eventFilter(sender, event);
}
