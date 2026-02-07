#include "kde-background-effect-manager.hpp"
#include <qapplication.h>
#include <qevent.h>
#include <qlogging.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include "qt-wayland-utils.hpp"

namespace KDE {

BackgroundEffectManager::BackgroundEffectManager(org_kde_kwin_blur_manager *manager)
    : m_manager(manager) {}

bool BackgroundEffectManager::supportsBlur() const { return true; }

bool BackgroundEffectManager::setBlur(QWindow *win, const BlurConfig &cfg) {
  if (auto it = m_state.find(win); it != m_state.end()) {
    if (it->second->cfg == cfg) return true;

    // region updating doesn't seem to work on kwin if the window is already
    // on screen, so we destroy and recreate the blur object every update
    m_state.erase(it);
  } else {
    win->installEventFilter(this);
  }

  auto *surface = QtWaylandUtils::getWindowSurface(win);

  if (!surface) {
    qWarning() << "Failed to get wl_surface for window" << win;
    return false;
  }

  auto *blur = org_kde_kwin_blur_manager_create(m_manager, surface);

  if (!blur) {
    qWarning() << "Failed to create blur object";
    return false;
  }

  auto state = std::make_unique<BlurState>(blur, cfg);

  applyBlur(win, *state);
  m_state[win] = std::move(state);

  return true;
}

bool BackgroundEffectManager::removeBlur(QWindow *win) {
  if (auto it = m_state.find(win); it != m_state.end()) {
    win->removeEventFilter(this);
    m_state.erase(it);
    return true;
  }

  return false;
}

bool BackgroundEffectManager::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::PlatformSurface) {
    auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);

    if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
      for (auto it = m_state.begin(); it != m_state.end(); ++it) {
        if (sender == it->first) {
          qDebug() << "Deleting effect for to-be-deleted window" << it->first;
          m_state.erase(it);
          return QObject::eventFilter(sender, event);
        }
      }
    }
  }

  return QObject::eventFilter(sender, event);
}

void BackgroundEffectManager::roundtrip() {
  wl_display_roundtrip(qApp->nativeInterface<QNativeInterface::QWaylandApplication>()->display());
}

void BackgroundEffectManager::applyBlur(QWindow *win, const BlurState &state) {
  auto region = QtWaylandUtils::createRoundedRegion(state.cfg.region, state.cfg.radius);
  org_kde_kwin_blur_set_region(state.blur, region);
  org_kde_kwin_blur_commit(state.blur);
  roundtrip();
}

} // namespace KDE
