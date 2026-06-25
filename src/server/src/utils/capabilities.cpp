#include "capabilities.hpp"
#include "environment.hpp"
#include "service-registry.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"
#include <array>
#include <utility>

namespace platform {

static constexpr std::array<std::pair<std::string_view, Capability>, 2> CAPABILITY_NAMES{{
    {"layerShell", Capability::LayerShell},
    {"globalShortcuts", Capability::GlobalShortcuts},
}};

bool supports(Capability cap) {
  switch (cap) {
  case Capability::LayerShell:
    return Environment::isLayerShellSupported();
  case Capability::GlobalShortcuts: {
    auto *service = ServiceRegistry::instance()->globalShortcuts();
    return service && service->isSupported();
  }
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
