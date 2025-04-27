#include "wm/hyprland/hyprland.hpp"
#include "wm/window-manager.hpp"
#include "wm/macos/macos-window-manager.hpp"
#include "wm/dummy-window-manager.hpp"
#include <qguiapplication.h>

class WindowManagerFactory {
public:
  std::unique_ptr<AbstractWindowManager> create() const {
    if (QGuiApplication::platformName() == "cocoa") return std::make_unique<MacOSWindowManager>();
    if (QGuiApplication::platformName() == "wayland") {
      {
        auto candidate = std::make_unique<HyprlandWindowManager>();

        if (candidate->isActivatable()) return candidate;
      }
    }

    return std::make_unique<DummyWindowManager>();
  }
};
