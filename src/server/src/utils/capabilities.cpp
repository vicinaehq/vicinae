#include "capabilities.hpp"
#include "environment.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "services/window-material/window-material-manager.hpp"
#include <array>
#include <utility>

#ifdef Q_OS_MACOS
bool macosLiquidGlassAvailable();
#endif

#ifdef Q_OS_WIN
bool windowsAcrylicSupported();
#endif

namespace platform {

static constexpr std::array<std::pair<std::string_view, Capability>, 9> CAPABILITY_NAMES{{
    {"layerShell", Capability::LayerShell},
    {"globalShortcuts", Capability::GlobalShortcuts},
    {"inputServer", Capability::InputServer},
    {"iconThemeSelection", Capability::IconThemeSelection},
    {"clientSideDecorations", Capability::ClientSideDecorations},
    {"liquidGlass", Capability::LiquidGlass},
    {"nativePanels", Capability::NativePanels},
    {"windowMaterial", Capability::WindowMaterial},
    {"customWindowRounding", Capability::CustomWindowRounding},
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
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
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
  case Capability::WindowMaterial: {
#if defined(Q_OS_MACOS)
    return true;
#elif defined(Q_OS_WIN)
    return windowsAcrylicSupported();
#else
    auto *mgr = ServiceRegistry::instance()->windowMaterialManager();
    return mgr && mgr->isSupported();
#endif
  }
  case Capability::CustomWindowRounding:
#ifdef Q_OS_WIN
    return !windowsAcrylicSupported();
#else
    return true;
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

bool preferItemPopup(std::string_view surface) {
#if defined(Q_OS_LINUX)
  return surface == "popover" || surface == "dialog";
#elif defined(Q_OS_WIN)
  // native popup windows misplace and flicker on Windows (QTBUG-120051)
  (void)surface;
  return true;
#else
  (void)surface;
  return false;
#endif
}

} // namespace platform
