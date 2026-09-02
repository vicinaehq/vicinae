#include "services/tray-host/tray-host.hpp"
#ifdef Q_OS_LINUX
#include "services/tray-host/sni/sni-tray-host.hpp"
#else
#include "services/tray-host/dummy-tray-host.hpp"
#endif

std::unique_ptr<AbstractTrayHost> createTrayHost() {
#ifdef Q_OS_LINUX
  return std::make_unique<SniTrayHost>();
#else
  return std::make_unique<DummyTrayHost>();
#endif
}
