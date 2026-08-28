#include "wayland-shortcut-inhibit-manager.hpp"
#include <QGuiApplication>
#include <qevent.h>
#include <qlogging.h>
#include "qt-wayland-utils.hpp"

KeyboardShortcutsInhibitV1::KeyboardShortcutsInhibitV1() : QWaylandClientExtensionTemplate(1) {
  initialize();
}

bool WaylandShortcutInhibitManager::inhibit(QWindow *win) {
  if (m_inhibitors.contains(win)) { return true; }
  if (!m_manager.isActive()) { return false; }

  auto *surface = QtWaylandUtils::getWindowSurface(win);

  if (!surface) {
    qWarning() << "Failed to get wl_surface for window" << win;
    return false;
  }

  auto *seat = qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->seat();
  auto *inhibitor = m_manager.inhibit_shortcuts(surface, seat);

  if (!inhibitor) { return false; }

  win->installEventFilter(this);
  m_inhibitors[win] = std::make_unique<Inhibitor>(inhibitor);

  return true;
}

bool WaylandShortcutInhibitManager::release(QWindow *win) {
  auto it = m_inhibitors.find(win);

  if (it == m_inhibitors.end()) { return false; }

  win->removeEventFilter(this);
  m_inhibitors.erase(it);

  return true;
}

bool WaylandShortcutInhibitManager::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::PlatformSurface) {
    auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT

    if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      if (auto it = m_inhibitors.find(qobject_cast<QWindow *>(sender)); it != m_inhibitors.end()) {
        m_inhibitors.erase(it);
      }
    }
  }

  return QObject::eventFilter(sender, event);
}
