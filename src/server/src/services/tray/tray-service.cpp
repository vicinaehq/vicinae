#include "services/tray/tray-service.hpp"
#ifdef Q_OS_MACOS
#include "services/tray/macos/tray-service-macos.hpp"
#elif defined(Q_OS_WIN)
#include "services/tray/windows/tray-service-windows.hpp"
#elif defined(Q_OS_LINUX)
#include "services/tray/linux/tray-service-linux.hpp"
#endif

std::unique_ptr<TrayService> createTrayService() {
#ifdef Q_OS_MACOS
  return std::make_unique<TrayServiceMacOS>();
#elif defined(Q_OS_WIN)
  return std::make_unique<TrayServiceWindows>();
#elif defined(Q_OS_LINUX)
  return std::make_unique<TrayServiceLinux>();
#else
  return nullptr;
#endif
}
