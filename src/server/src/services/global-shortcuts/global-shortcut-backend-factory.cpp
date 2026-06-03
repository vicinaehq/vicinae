#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#include "services/global-shortcuts/dummy-global-shortcut-backend.hpp"
#include "services/global-shortcuts/ext-hotkey-global-shortcut-backend.hpp"
#include "wayland/globals.hpp"
#include <memory>

#ifdef Q_OS_MACOS
#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#endif

std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend() {
  // TODO: X11 (XGrabKey) / Windows backends.
#ifdef Q_OS_MACOS
  return std::make_unique<MacOSGlobalShortcutBackend>();
#elifdef Q_OS_LINUX
  if (Wayland::Globals::hotkey()) { return std::make_unique<ExtHotkeyGlobalShortcutBackend>(); }
#endif
  return std::make_unique<DummyGlobalShortcutBackend>();
}
