#include "services/layout-switch/layout-switch-service-factory.hpp"
#include "services/layout-switch/abstract-layout-switch-service.hpp"

#ifdef Q_OS_MACOS
#include "services/layout-switch/macos-layout-switch-service.hpp"
#endif

std::unique_ptr<AbstractLayoutSwitchService> createLayoutSwitchService() {
#ifdef Q_OS_MACOS
  return std::make_unique<MacosLayoutSwitchService>();
#else
  return std::make_unique<AbstractLayoutSwitchService>();
#endif
}
