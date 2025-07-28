#include "wm/hyprland/hyprland.hpp"
#include "wm/window-manager.hpp"
#include "wm/macos/macos-window-manager.hpp"
#include "wm/dummy-window-manager.hpp"
#include <qguiapplication.h>

class WindowManagerFactory {
public:
  std::unique_ptr<AbstractWindowManager> create() const {
#if defined Q_OS_DARWIN
    return std::make_unique<MacOSWindowManager>();
#endif

#if defined(Q_OS_UNIX) && not defined(Q_OS_DARWIN)
    if (QGuiApplication::platformName() == "wayland") {
      {
        auto candidate = std::make_unique<HyprlandWindowManager>();

        if (candidate->isActivatable()) return candidate;
      }
    }
#endif

    return std::make_unique<DummyWindowManager>();
  }
};
