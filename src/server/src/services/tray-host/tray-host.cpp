#include "services/tray-host/tray-host.hpp"
#ifdef Q_OS_LINUX
#include "services/tray-host/sni/sni-tray-host.hpp"
#else
#include "services/tray-host/dummy-tray-host.hpp"
#endif

std::unique_ptr<AbstractTrayHost> createTrayHost([[maybe_unused]] bool watcherEnabled) {
#ifdef Q_OS_LINUX
  return std::make_unique<SniTrayHost>(watcherEnabled);
#else
  return std::make_unique<DummyTrayHost>();
#endif
}
