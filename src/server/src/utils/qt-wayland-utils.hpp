#pragma once
#include <qapplication.h>
#include <qwindow.h>
#include <wayland-client-protocol.h>
#include <qpa/qplatformnativeinterface.h>

namespace QtWaylandUtils {
struct ScopedRegion {
  explicit ScopedRegion(wl_region *region) : m_region(region) {}
  ~ScopedRegion() {
    if (m_region) wl_region_destroy(m_region);
  }

  operator wl_region *() const { return m_region; }

  wl_region *m_region;
};

inline wl_surface *getWindowSurface(QWindow *win) {
  if (QApplication::platformName() != "wayland") return nullptr;
  auto wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  auto iface = qApp->platformNativeInterface();
  return static_cast<wl_surface *>(iface->nativeResourceForWindow("surface", win));
}

inline ScopedRegion createRoundedRegion(QRect region, int radius) {
  auto wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  const auto wlregion = wl_compositor_create_region(wayland->compositor());
  int w = region.width();
  int h = region.height();
  int r = radius;

  wl_region_add(wlregion, region.x(), region.y(), w, h);

  for (int i = 0; i < r; i++) {
    int cut = r - static_cast<int>(std::sqrt(static_cast<double>((r * r) - ((r - i) * (r - i)))));

    if (cut <= 0) continue;

    wl_region_subtract(wlregion, 0, i, cut, 1);
    wl_region_subtract(wlregion, w - cut, i, cut, 1);
    wl_region_subtract(wlregion, 0, h - 1 - i, cut, 1);
    wl_region_subtract(wlregion, w - cut, h - 1 - i, cut, 1);
  }

  return ScopedRegion(wlregion);
}

}; // namespace QtWaylandUtils
