#pragma once
#include <QtGlobal>
#include <string_view>

namespace platform {

constexpr std::string_view extensionPlatform() {
#if defined(Q_OS_MACOS)
  return "macOS";
#elif defined(Q_OS_WIN)
  return "Windows";
#else
  return "linux";
#endif
}

/**
 * A runtime fact about what the current install can do, as opposed to which OS
 * it was built for. A capability may be false even on the platform it belongs
 * to (e.g. LayerShell is unavailable on an X11 Linux session).
 */
enum class Capability {
  LayerShell,
  GlobalShortcuts,
  InputServer,
  IconThemeSelection,
  ClientSideDecorations,
  LiquidGlass,
  NativePanels,
  WindowMaterial,
};

bool supports(Capability cap);

/**
 * Resolve a capability by its QML-facing name. Unknown names resolve to false.
 */
bool supports(std::string_view name);

/**
 * Some surfaces can be represented either as native popup windows or in-scene
 * popup items. Keep the platform policy centralized so QML components do not
 * need to encode OS checks.
 */
bool preferItemPopup(std::string_view surface);

} // namespace platform
