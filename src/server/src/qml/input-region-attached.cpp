#include "input-region-attached.hpp"
#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>

#ifdef Q_OS_LINUX
#include <QGuiApplication>
#include "qt-wayland-utils.hpp"
#endif

InputRegionAttached::InputRegionAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }

  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &InputRegionAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

void InputRegionAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  apply();
}

void InputRegionAttached::setRegion(const QRect &value) {
  if (m_region == value) return;
  m_region = value;
  emit regionChanged();
  apply();
}

void InputRegionAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
}

void InputRegionAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    m_window = nullptr;
    m_surfaceReady = false;
    m_applied = false;
  }

  if (window) {
    trackWindow(window);
    apply();
  }
}

void InputRegionAttached::apply() {
#ifdef Q_OS_LINUX
  if (!m_window) return;

  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  auto *surface = QtWaylandUtils::getWindowSurface(m_window);
  if (!surface) return;

  if (m_enabled && !m_region.isEmpty()) {
    auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (!wayland) return;
    QtWaylandUtils::ScopedRegion region(wl_compositor_create_region(wayland->compositor()));
    wl_region_add(region, m_region.x(), m_region.y(), m_region.width(), m_region.height());
    wl_surface_set_input_region(surface, region);
    m_applied = true;
  } else if (m_applied) {
    wl_surface_set_input_region(surface, nullptr);
    m_applied = false;
  }
#endif
}

bool InputRegionAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      m_applied = false;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
      m_applied = false;
    }
  }
  return QObject::eventFilter(obj, event);
}
