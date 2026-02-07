#pragma once
#include <qapplication.h>
#include <qwindow.h>
#include <wayland-client-protocol.h>
#include <qpa/qplatformnativeinterface.h>

namespace QtWaylandUtils {
inline wl_surface *getWindowSurface(QWindow *win) {
  if (QApplication::platformName() != "wayland") return nullptr;
  auto wayland = qApp->nativeInterface<QNativeInterface::QWaylandApplication>();
  auto iface = qApp->platformNativeInterface();
  return static_cast<wl_surface *>(iface->nativeResourceForWindow("surface", win));
}
}; // namespace QtWaylandUtils
