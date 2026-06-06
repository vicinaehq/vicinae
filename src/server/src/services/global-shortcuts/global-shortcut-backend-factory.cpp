#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#include "services/global-shortcuts/dummy-global-shortcut-backend.hpp"
#include <memory>

#ifdef Q_OS_MACOS
#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#elifdef Q_OS_LINUX
#include "services/global-shortcuts/ext-hotkey-global-shortcut-backend.hpp"
#include "services/global-shortcuts/x11-global-shortcut-backend.hpp"
#include "wayland/globals.hpp"
#include <QGuiApplication>
#endif

std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend() {
  // TODO: Windows backend.
#ifdef Q_OS_MACOS
  return std::make_unique<MacOSGlobalShortcutBackend>();
#elifdef Q_OS_LINUX
  // A real X11 session reports the "xcb" platform; under Wayland Qt defaults to the wayland plugin.
  // XGrabKey on the root window is not truly global under XWayland, so X11 is only used for "xcb".
  if (QGuiApplication::platformName() == "xcb") { return std::make_unique<X11GlobalShortcutBackend>(); }
  if (Wayland::Globals::hotkey()) { return std::make_unique<ExtHotkeyGlobalShortcutBackend>(); }
#endif
  return std::make_unique<DummyGlobalShortcutBackend>();
}
