#include "services/global-shortcuts/global-shortcut-backend-factory.hpp"
#include "environment.hpp"
#include "services/global-shortcuts/dummy-global-shortcut-backend.hpp"
#include <memory>

#ifdef Q_OS_MACOS
#include "services/global-shortcuts/macos-global-shortcut-backend.hpp"
#elifdef Q_OS_WIN
#include "services/global-shortcuts/windows-global-shortcut-backend.hpp"
#elifdef Q_OS_LINUX
#include "services/global-shortcuts/vicinae-hotkey-global-shortcut-backend.hpp"
#include "services/global-shortcuts/x11-global-shortcut-backend.hpp"
#include "wayland/globals.hpp"
#include <QGuiApplication>
#endif

std::unique_ptr<AbstractGlobalShortcutBackend> createGlobalShortcutBackend() {
#ifdef Q_OS_MACOS
  return std::make_unique<MacOSGlobalShortcutBackend>();
#elifdef Q_OS_WIN
  return std::make_unique<WindowsGlobalShortcutBackend>();
#elifdef Q_OS_LINUX
  if (Environment::isX11()) { return std::make_unique<X11GlobalShortcutBackend>(); }
  if (Wayland::Globals::hotkey()) { return std::make_unique<VicinaeHotkeyGlobalShortcutBackend>(); }
#endif
  return std::make_unique<DummyGlobalShortcutBackend>();
}
