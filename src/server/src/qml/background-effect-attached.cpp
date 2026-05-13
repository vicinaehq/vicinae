#include "background-effect-attached.hpp"
#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>

BackgroundEffectAttached::BackgroundEffectAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }

  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &BackgroundEffectAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

void BackgroundEffectAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  apply();
}

void BackgroundEffectAttached::setRadius(int value) {
  if (m_radius == value) return;
  m_radius = value;
  emit radiusChanged();
  apply();
}

bool BackgroundEffectAttached::supportsRegionalBlur() {
  auto *mgr = BackgroundEffect::manager();
  return mgr && mgr->supportsBlur();
}

void BackgroundEffectAttached::setRegion(const QRect &value) {
  if (m_hasExplicitRegion && m_region == value) return;
  m_region = value;
  m_hasExplicitRegion = true;
  emit regionChanged();
  apply();
}

void BackgroundEffectAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
  connect(m_window, &QWindow::widthChanged, this, &BackgroundEffectAttached::apply);
  connect(m_window, &QWindow::heightChanged, this, &BackgroundEffectAttached::apply);
}

void BackgroundEffectAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    disconnect(m_window, &QWindow::widthChanged, this, &BackgroundEffectAttached::apply);
    disconnect(m_window, &QWindow::heightChanged, this, &BackgroundEffectAttached::apply);
    auto *mgr = BackgroundEffect::manager();
    if (mgr && m_surfaceReady) mgr->clearBlur(m_window);
    m_window = nullptr;
    m_surfaceReady = false;
  }

  if (window) {
    trackWindow(window);
    apply();
  }
}

void BackgroundEffectAttached::apply() {
  auto *mgr = BackgroundEffect::manager();
  if (!m_window || !mgr || !mgr->supportsBlur()) return;

  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  if (m_enabled) {
    mgr->setBlur(m_window, {.radius = m_radius, .region = effectiveRegion()});
  } else {
    mgr->clearBlur(m_window);
  }
}

QRect BackgroundEffectAttached::effectiveRegion() const {
  if (m_hasExplicitRegion) return m_region;
  if (m_window) return {0, 0, m_window->width(), m_window->height()};
  return {};
}

bool BackgroundEffectAttached::eventFilter(QObject *obj, QEvent *event) {
  if (obj == m_window && event->type() == QEvent::PlatformSurface) {
    auto *se = static_cast<QPlatformSurfaceEvent *>(event); // NOLINT
    if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceCreated) {
      m_surfaceReady = true;
      apply();
    } else if (se->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      m_surfaceReady = false;
    }
  }
  return QObject::eventFilter(obj, event);
}
