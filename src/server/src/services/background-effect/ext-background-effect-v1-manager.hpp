#pragma once
#include <qcoreevent.h>
#include <qevent.h>
#include <qobject.h>
#include <qwindow.h>
#include <unordered_map>
#include <qapplication.h>
#include <qpa/qplatformnativeinterface.h>
#include <wayland-client-core.h>
#include "ext-background-effect-v1-client-protocol.h"
#include "qt-wayland-utils.hpp"
#include "services/background-effect/abstract-background-effect-manager.hpp"

class ExtBackgroundEffectV1Manager : public AbstractBackgroundEffectManager {
  static void capabilities(void *data,
                           struct ext_background_effect_manager_v1 *ext_background_effect_manager_v1,
                           uint32_t flags) {
    static_cast<ExtBackgroundEffectV1Manager *>(data)->m_supportsBlur =
        flags & EXT_BACKGROUND_EFFECT_MANAGER_V1_CAPABILITY_BLUR;
  }

  static constexpr ext_background_effect_manager_v1_listener listener = {.capabilities = capabilities};

public:
  ExtBackgroundEffectV1Manager(ext_background_effect_manager_v1 *manager) : m_manager(manager) {
    const auto wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    ext_background_effect_manager_v1_add_listener(manager, &listener, this);
    wl_display_roundtrip(wayland->display());
  }

  bool eventFilter(QObject *sender, QEvent *event) override {
    // We must destroy the effect object before the wl_surface gets destroyed.
    // PlatformSurface/SurfaceAboutToBeDestroyed fires right before the native
    // surface is torn down, regardless of how the window is deleted.
    if (event->type() == QEvent::PlatformSurface) {
      auto *surfaceEvent = static_cast<QPlatformSurfaceEvent *>(event);
      if (surfaceEvent->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
        for (auto it = m_state.begin(); it != m_state.end(); ++it) {
          if (sender == it->first && it->second) {
            qInfo() << "Deleting effect for to-be-deleted window" << it->first;
            ext_background_effect_surface_v1_destroy(it->second);
            m_state.erase(it);
            return QObject::eventFilter(sender, event);
          }
        }
      }
    }

    return QObject::eventFilter(sender, event);
  }

  ~ExtBackgroundEffectV1Manager() {
    for (const auto &[k, effect] : m_state) {
      ext_background_effect_surface_v1_destroy(effect);
    }
  }

  bool supportsBlur() const override { return m_supportsBlur; }

  bool setBlur(QWindow *win, const BlurConfig &cfg) override {
    if (!m_supportsBlur) { return false; }

    win->installEventFilter(this);

    if (auto it = m_state.find(win); it != m_state.end()) {
      ext_background_effect_surface_v1_destroy(it->second);
      it->second = nullptr;
    }

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

    applyBlur(win, effect, cfg.region, cfg.radius);
    m_state[win] = effect;

    return true;
  }

  bool removeBlur(QWindow *win) override {
    if (!m_supportsBlur) { return false; }

    if (auto it = m_state.find(win); it != m_state.end()) {
      m_state.erase(it);
      return true;
    }

    return false;
  }

private:
  void applyBlur(QWindow *win, ext_background_effect_surface_v1 *wef, const QRect &rect, int radius) {
    const auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    const auto region = wl_compositor_create_region(wayland->compositor());
    int w = rect.width();
    int h = rect.height();
    int r = radius;

    wl_region_add(region, rect.x(), rect.y(), w, h);

    // account for client side rounding
    for (int i = 0; i < r; i++) {
      int cut = r - static_cast<int>(std::sqrt(static_cast<double>((r * r) - ((r - i) * (r - i)))));

      if (cut <= 0) continue;

      wl_region_subtract(region, 0, i, cut, 1);
      wl_region_subtract(region, w - cut, i, cut, 1);
      wl_region_subtract(region, 0, h - 1 - i, cut, 1);
      wl_region_subtract(region, w - cut, h - 1 - i, cut, 1);
    }

    ext_background_effect_surface_v1_set_blur_region(wef, region);
    wl_region_destroy(region);
  }

  ext_background_effect_manager_v1 *m_manager = nullptr;
  std::unordered_map<QWindow *, ext_background_effect_surface_v1 *> m_state;
  bool m_supportsBlur = false;
};
