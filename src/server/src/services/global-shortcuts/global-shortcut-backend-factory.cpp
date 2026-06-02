#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#include "services/global-shortcuts/dummy-global-shortcut-backend.hpp"

std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend() {
  // TODO: select an X11 (XGrabKey) / macOS / Windows backend here. Wayland has no mechanism we're
  // willing to support, so it falls through to the dummy (global shortcuts reported as unsupported).
  return std::make_unique<DummyGlobalShortcutBackend>();
}
