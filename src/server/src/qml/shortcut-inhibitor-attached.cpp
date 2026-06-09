#include "shortcut-inhibitor-attached.hpp"
#include <QQuickItem>
#include <QQuickWindow>
#include <qevent.h>

ShortcutInhibitorAttached::ShortcutInhibitorAttached(QObject *parent) : QObject(parent) {
  m_window = qobject_cast<QWindow *>(parent);
  if (m_window) {
    trackWindow(m_window);
    return;
  }

  m_item = qobject_cast<QQuickItem *>(parent);
  if (m_item) {
    connect(m_item, &QQuickItem::windowChanged, this, &ShortcutInhibitorAttached::onWindowChanged);
    if (m_item->window()) onWindowChanged(m_item->window());
  }
}

void ShortcutInhibitorAttached::setEnabled(bool value) {
  if (m_enabled == value) return;
  m_enabled = value;
  emit enabledChanged();
  apply();
}

void ShortcutInhibitorAttached::trackWindow(QWindow *window) {
  m_window = window;
  if (!m_window) return;
  m_window->installEventFilter(this);
}

void ShortcutInhibitorAttached::onWindowChanged(QQuickWindow *window) {
  if (m_window) {
    m_window->removeEventFilter(this);
    if (auto *mgr = ShortcutInhibitor::manager(); mgr && m_inhibited) { mgr->release(m_inhibited); }
    m_inhibited = nullptr;
    m_window = nullptr;
    m_surfaceReady = false;
  }

  if (window) {
    trackWindow(window);
    apply();
  }
}

QWindow *ShortcutInhibitorAttached::targetWindow() const {
  QWindow *win = m_window;
  while (win && win->transientParent()) {
    win = win->transientParent();
  }
  return win;
}

void ShortcutInhibitorAttached::apply() {
  auto *mgr = ShortcutInhibitor::manager();
  QWindow *target = targetWindow();

  if (!m_window || !mgr || !mgr->isSupported()) return;

  if (!m_surfaceReady) {
    if (!m_window->handle()) return;
    m_surfaceReady = true;
  }

  if (m_inhibited && (!m_enabled || m_inhibited != target)) {
    mgr->release(m_inhibited);
    m_inhibited = nullptr;
  }

  if (m_enabled && target && target->handle() && !m_inhibited) {
    if (mgr->inhibit(target)) { m_inhibited = target; }
  }
}

bool ShortcutInhibitorAttached::eventFilter(QObject *obj, QEvent *event) {
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
