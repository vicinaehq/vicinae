#pragma once
#include <qwindow.h>
#include <unordered_map>
#include <qapplication.h>
#include <qpa/qplatformnativeinterface.h>
#include <wayland-client-core.h>
#include "ext-background-effect-v1-client-protocol.h"
#include "qt-wayland-utils.hpp"
#include "services/background-effect/abstract-background-effect-manager.hpp"
#include "wayland/globals.hpp"

class ExtBackgroundEffectV1Manager : public AbstractBackgroundEffectManager {
  class WindowEffect {
    using Effect = ext_background_effect_surface_v1;

  public:
    WindowEffect(Effect *effect) : m_effect(effect) {}
    ~WindowEffect() {
      if (m_effect) ext_background_effect_surface_v1_destroy(m_effect);
    }

    auto effect() const { return m_effect; }

  private:
    Effect *m_effect;
  };

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

  bool supportsBlur() const override { return m_supportsBlur; }

  bool setBlur(QWindow *win, const BlurConfig &cfg) override {
    if (!m_supportsBlur) { return false; }

    // just change region if we already have blur
    if (auto it = m_state.find(win); it != m_state.end()) {
      applyBlur(win, *it->second, cfg.radius);
      return true;
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

    m_state[win] = std::make_unique<WindowEffect>(effect);
    applyBlur(win, effect, cfg.radius);

    return true;
  }

  bool removeBlur(QWindow *win) override {
    auto *manager = Wayland::Globals::kwinBlur();
    if (!manager) return false;

    if (auto it = m_state.find(win); it != m_state.end()) {
      m_state.erase(it);
      return true;
    }

    return false;
  }

private:
  void applyBlur(QWindow *win, const WindowEffect &wef, int radius) {
    const auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    const auto region = wl_compositor_create_region(wayland->compositor());
    int w = win->width();
    int h = win->height();
    int r = radius;

    wl_region_add(region, 0, 0, w, h);

    // account for client side rounding
    for (int i = 0; i < r; i++) {
      int cut = r - static_cast<int>(std::sqrt(static_cast<double>((r * r) - ((r - i) * (r - i)))));

      if (cut <= 0) continue;

      wl_region_subtract(region, 0, i, cut, 1);
      wl_region_subtract(region, w - cut, i, cut, 1);
      wl_region_subtract(region, 0, h - 1 - i, cut, 1);
      wl_region_subtract(region, w - cut, h - 1 - i, cut, 1);
    }

    ext_background_effect_surface_v1_set_blur_region(wef.effect(), region);
    wl_region_destroy(region);
  }

  ext_background_effect_manager_v1 *m_manager = nullptr;
  std::unordered_map<QWindow *, std::unique_ptr<WindowEffect>> m_state;
  bool m_supportsBlur = false;
};
