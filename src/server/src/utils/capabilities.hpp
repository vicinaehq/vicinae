#pragma once
#include <string_view>

namespace platform {

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
};

bool supports(Capability cap);

/**
 * Resolve a capability by its QML-facing name. Unknown names resolve to false.
 */
bool supports(std::string_view name);

} // namespace platform
