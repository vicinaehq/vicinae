#include "ext-background-effect-v1-manager.hpp"
#include <QGuiApplication>
#include <qevent.h>
#include <qlogging.h>
#include <wayland-client-core.h>
#include "qt-wayland-utils.hpp"

ExtBackgroundEffectV1::ExtBackgroundEffectV1() : QWaylandClientExtensionTemplate(1) {
  initialize();

  // roundtrip so the one-shot capabilities event is delivered before we report support
  if (isActive()) {
    auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    wl_display_roundtrip(wayland->display());
  }
}

void ExtBackgroundEffectV1::ext_background_effect_manager_v1_capabilities(uint32_t flags) {
  m_capabilities = flags;
}

bool ExtBackgroundEffectV1Manager::isSupported() const {
  return m_manager.isActive() && m_manager.supportsBlur();
}

bool ExtBackgroundEffectV1Manager::apply(QWindow *win, const Params &params) {
  if (!isSupported()) { return false; }

  if (auto it = m_state.find(win); it != m_state.end()) {
    auto &state = it->second;

    if (state->cfg != params) {
      state->cfg = params;
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

  auto *effect = m_manager.get_background_effect(surface);

  if (!effect) {
    qWarning() << "Failed to create background effect object";
    return false;
  }

  auto state = std::make_unique<BlurState>(effect, params);

  applyBlur(win, *state);
  m_state[win] = std::move(state);

  return true;
}

bool ExtBackgroundEffectV1Manager::clear(QWindow *win) {
  if (auto it = m_state.find(win); it != m_state.end()) {
    win->removeEventFilter(this);
    m_state.erase(it);
    return true;
  }

  return false;
}

bool ExtBackgroundEffectV1Manager::eventFilter(QObject *sender, QEvent *event) {
  if (event->type() == QEvent::PlatformSurface) {
    auto *surfaceEvent =
        static_cast<QPlatformSurfaceEvent *>( // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            event);

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

void ExtBackgroundEffectV1Manager::applyBlur(QWindow *, BlurState &state) {
  const auto region = QtWaylandUtils::createRoundedRegion(state.cfg.region, state.cfg.radius);
  state.effect.set_blur_region(region);
}
