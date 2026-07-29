#include "abstract-wayland-window-manager.hpp"
#include "internal/wayland/globals.hpp"

std::vector<AbstractWindowManager::Screen>
AbstractWaylandWindowManager::listScreensSync(QWindow *activeWindow) const {
  auto screens = AbstractWindowManager::listScreensSync(activeWindow);

  // the DPR-derived default is wrong under fractional scaling, the current output mode is ground truth
  for (auto &screen : screens) {
    if (auto size = Wayland::Globals::outputPixelSize(screen.name.toStdString())) {
      screen.physicalResolution = *size;
    }
  }

  return screens;
}
