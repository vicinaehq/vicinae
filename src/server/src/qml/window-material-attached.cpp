#include "window-material-attached.hpp"
#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>

WindowMaterialAttached::WindowMaterialAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }

  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &WindowMaterialAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

void WindowMaterialAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  apply();
}

void WindowMaterialAttached::setRadius(int value) {
  if (m_radius == value) return;
  m_radius = value;
  emit radiusChanged();
  apply();
}

bool WindowMaterialAttached::supportsRegionalBlur() {
  auto *mgr = WindowMaterial::manager();
  return mgr && mgr->isSupported();
}

void WindowMaterialAttached::setRegion(const QRect &value) {
  if (m_hasExplicitRegion && m_region == value) return;
  m_region = value;
  m_hasExplicitRegion = true;
  emit regionChanged();
  apply();
}

void WindowMaterialAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
  connect(m_window, &QWindow::widthChanged, this, &WindowMaterialAttached::apply);
  connect(m_window, &QWindow::heightChanged, this, &WindowMaterialAttached::apply);
}

void WindowMaterialAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    disconnect(m_window, &QWindow::widthChanged, this, &WindowMaterialAttached::apply);
    disconnect(m_window, &QWindow::heightChanged, this, &WindowMaterialAttached::apply);
    auto *mgr = WindowMaterial::manager();
    if (mgr && m_surfaceReady && m_applied) mgr->clear(m_window);
    m_window = nullptr;
    m_surfaceReady = false;
    m_applied = false;
  }

  if (window) {
    trackWindow(window);
    apply();
  }
}

void WindowMaterialAttached::apply() {
  auto *mgr = WindowMaterial::manager();
  if (!m_window || !mgr || !mgr->isSupported()) return;

  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  if (m_enabled) {
    mgr->apply(m_window, {.radius = m_radius, .region = effectiveRegion()});
    m_applied = true;
  } else if (m_applied) {
    mgr->clear(m_window);
    m_applied = false;
  }
}

QRect WindowMaterialAttached::effectiveRegion() const {
  if (m_hasExplicitRegion) return m_region;
  if (m_window) return {0, 0, m_window->width(), m_window->height()};
  return {};
}

bool WindowMaterialAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
      m_applied = false;
    }
  }
  return QObject::eventFilter(obj, event);
}
