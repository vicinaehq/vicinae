#include "layout-resolver.hpp"

#ifdef Q_OS_MACOS
#include "keyboard-macos.hpp"
#elifdef Q_OS_WIN
#include "windows-layout-resolver.hpp"
#elifdef Q_OS_LINUX
#include "environment.hpp"
#include "wayland-layout-resolver.hpp"
#include "x11-layout-resolver.hpp"
#endif

namespace Keyboard {

std::unique_ptr<LayoutResolver> createLayoutResolver() {
#ifdef Q_OS_MACOS
  return std::make_unique<MacosLayoutResolver>();
#elifdef Q_OS_WIN
  return std::make_unique<WindowsLayoutResolver>();
#elifdef Q_OS_LINUX
  if (Environment::isX11()) { return std::make_unique<X11LayoutResolver>(); }
  if (Environment::isWaylandSession()) { return std::make_unique<WaylandLayoutResolver>(); }
#endif
  return nullptr;
}

} // namespace Keyboard
