#include "ext-background-effect-v1-manager.hpp"
#include <qapplication.h>
#include <qevent.h>
#include <qlogging.h>
#include <wayland-client-core.h>
#include "qt-wayland-utils.hpp"

void ExtBackgroundEffectV1Manager::capabilities(void *data, ext_background_effect_manager_v1 *,
                                                uint32_t flags) {
  static_cast<ExtBackgroundEffectV1Manager *>(data)->m_supportsBlur =
      flags & EXT_BACKGROUND_EFFECT_MANAGER_V1_CAPABILITY_BLUR;
}

ExtBackgroundEffectV1Manager::ExtBackgroundEffectV1Manager(ext_background_effect_manager_v1 *manager)
    : m_manager(manager) {
  auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  ext_background_effect_manager_v1_add_listener(manager, &s_listener, this);
  wl_display_roundtrip(wayland->display());
}

bool ExtBackgroundEffectV1Manager::supportsBlur() const { return m_supportsBlur; }

bool ExtBackgroundEffectV1Manager::setBlur(QWindow *win, const BlurConfig &cfg) {
  if (!m_supportsBlur) { return false; }

  if (auto it = m_state.find(win); it != m_state.end()) {
    auto &state = it->second;

    if (state->cfg != cfg) {
      state->cfg = cfg;
      applyBlur(win, *state);
    }

    return true;
  }

  win->installEventFilter(this);

  auto *surface = QtWaylandUtils::getWindowSurface(win);

  if (!surface) {
    qWarning() << "Failed to get wl_surface for window" << win;
    return false;
  }

  auto effect = ext_background_effect_manager_v1_get_background_effect(m_manager, surface);

  if (!effect) {
    qWarning() << "Failed to create background effect object";
    return false;
  }

  auto state = std::make_unique<BlurState>(effect, cfg);

  applyBlur(win, *state);
  m_state[win] = std::move(state);

  return true;
}

bool ExtBackgroundEffectV1Manager::removeBlur(QWindow *win) {
  if (auto it = m_state.find(win); it != m_state.end()) {
    win->removeEventFilter(this);
    m_state.erase(it);
    return true;
  }

  return false;
}

bool ExtBackgroundEffectV1Manager::eventFilter(QObject *sender, QEvent *event) {
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

void ExtBackgroundEffectV1Manager::applyBlur(QWindow *win, const BlurState &state) {
  const auto region = QtWaylandUtils::createRoundedRegion(state.cfg.region, state.cfg.radius);
  ext_background_effect_surface_v1_set_blur_region(state.effect, region);
}
