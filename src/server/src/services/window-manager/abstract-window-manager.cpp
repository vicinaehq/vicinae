#include "abstract-window-manager.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <ranges>

#ifdef Q_OS_LINUX
#include "internal/wayland/globals.hpp"
#endif

namespace {

QSize physicalResolutionForScreen(const QScreen *screen) {
#ifdef Q_OS_LINUX
  // wl_output is the ground truth on wayland: the size reconstructed from the device pixel ratio
  // can be off by a pixel under fractional scaling
  if (auto size = Wayland::Globals::outputPixelSize(screen->name().toStdString())) { return *size; }
#endif
  return screen->size() * screen->devicePixelRatio();
}

} // namespace

std::vector<AbstractWindowManager::Screen> AbstractWindowManager::listScreensSync() const {
  auto tr = [](const QScreen *qtScreen) -> Screen {
    Screen sc{.name = qtScreen->name(),
              .bounds = qtScreen->geometry(),
              .physicalResolution = physicalResolutionForScreen(qtScreen),
              .manufacturer = qtScreen->manufacturer(),
              .model = qtScreen->model()};

    if (auto serial = qtScreen->serialNumber(); !serial.isEmpty()) { sc.serial = serial; }
    return sc;
  };
  return QGuiApplication::screens() | std::views::transform(tr) | std::ranges::to<std::vector>();
}
