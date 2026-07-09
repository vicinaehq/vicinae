#include "abstract-wayland-window-manager.hpp"
#include "internal/wayland/globals.hpp"

std::vector<AbstractWindowManager::Screen> AbstractWaylandWindowManager::listScreensSync() const {
  auto screens = AbstractWindowManager::listScreensSync();

  // the DPR-derived default is wrong under fractional scaling, the current output mode is ground truth
  for (auto &screen : screens) {
    if (auto size = Wayland::Globals::outputPixelSize(screen.name.toStdString())) {
      screen.physicalResolution = QSize(size->width, size->height);
    }
  }

  return screens;
}
