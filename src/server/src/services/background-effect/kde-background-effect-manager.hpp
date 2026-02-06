#pragma once
#include <qwindow.h>
#include <unordered_map>
#include <qpa/qplatformnativeinterface.h>
#include "kde-blur-client-protocol.h"
#include "qt-wayland-utils.hpp"
#include "services/background-effect/abstract-background-effect-manager.hpp"
#include "wayland/globals.hpp"
#include <qapplication.h>

namespace KDE {
class BackgroundEffectManager : public AbstractBackgroundEffectManager {
public:
  ~BackgroundEffectManager() {
    for (const auto &[win, blur] : m_state) {
      org_kde_kwin_blur_release(blur);
    }
  }

  bool setBlur(QWindow *win, const BlurConfig &cfg) override {
    auto *manager = Wayland::Globals::kwinBlur();

    if (!manager) return false;

    // just change region if we already have blur
    if (auto it = m_state.find(win); it != m_state.end()) {
      applyBlur(win, it->second, cfg.radius);
      return true;
    }

    auto *surface = QtWaylandUtils::getWindowSurface(win);
    auto *blur = org_kde_kwin_blur_manager_create(manager, surface);

    if (!blur) return false;

    m_state[win] = blur;
    applyBlur(win, blur, cfg.radius);

    return true;
  }

  bool removeBlur(QWindow *win) override {
    auto *manager = Wayland::Globals::kwinBlur();
    if (!manager) return false;

    if (auto it = m_state.find(win); it != m_state.end()) {
      org_kde_kwin_blur_release(it->second);
      m_state.erase(it);
      return true;
    }

    return false;
  }

private:
  void applyBlur(QWindow *win, org_kde_kwin_blur *blur, int radius) {
    auto *wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
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

    org_kde_kwin_blur_set_region(blur, region);
    org_kde_kwin_blur_commit(blur);
    wl_region_destroy(region);
  }

  std::unordered_map<QWindow *, org_kde_kwin_blur *> m_state;
  org_kde_kwin_blur_manager *m_manager = nullptr;
};
}; // namespace KDE
