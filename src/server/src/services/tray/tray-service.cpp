#include "services/tray/tray-service.hpp"
#ifdef Q_OS_MACOS
#include "services/tray/macos/tray-service-macos.hpp"
#endif

std::unique_ptr<TrayService> createTrayService() {
#ifdef Q_OS_MACOS
  return std::make_unique<TrayServiceMacOS>();
#else
  return nullptr;
#endif
}
