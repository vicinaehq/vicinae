#include "capabilities.hpp"
#include "environment.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include <array>
#include <utility>

#ifdef Q_OS_MACOS
bool macosLiquidGlassAvailable();
#endif

namespace platform {

static constexpr std::array<std::pair<std::string_view, Capability>, 7> CAPABILITY_NAMES{{
    {"layerShell", Capability::LayerShell},
    {"globalShortcuts", Capability::GlobalShortcuts},
    {"inputServer", Capability::InputServer},
    {"iconThemeSelection", Capability::IconThemeSelection},
    {"clientSideDecorations", Capability::ClientSideDecorations},
    {"liquidGlass", Capability::LiquidGlass},
    {"nativePanels", Capability::NativePanels},
}};

bool supports(Capability cap) {
  switch (cap) {
  case Capability::LayerShell:
    return Environment::isLayerShellSupported();
  case Capability::GlobalShortcuts: {
    auto *service = ServiceRegistry::instance()->globalShortcuts();
    return service && service->isSupported();
  }
  case Capability::InputServer:
  case Capability::IconThemeSelection:
#ifdef Q_OS_LINUX
    return true;
#else
    return false;
#endif
  case Capability::ClientSideDecorations:
#ifdef Q_OS_MACOS
    return false;
#else
    return true;
#endif
  case Capability::LiquidGlass:
#ifdef Q_OS_MACOS
    return macosLiquidGlassAvailable();
#else
    return false;
#endif
  case Capability::NativePanels:
#ifdef Q_OS_MACOS
    return true;
#else
    return false;
#endif
  }

  return false;
}

bool supports(std::string_view name) {
  for (const auto &[key, cap] : CAPABILITY_NAMES) {
    if (key == name) return supports(cap);
  }

  return false;
}

} // namespace platform
