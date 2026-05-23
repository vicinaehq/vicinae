#include "app-runtime.hpp"

#ifdef Q_OS_MACOS
#include "services/app-runtime/macos/mac-app-runtime.hpp"
#else
#include "services/app-runtime/linux/linux-app-runtime.hpp"
#endif

AppRuntime::AppRuntime(WindowManager &wm) : m_provider(createProvider(wm)) {
  connect(m_provider.get(), &AbstractAppRuntime::runningAppsChanged, this, &AppRuntime::runningAppsChanged);
}

bool AppRuntime::isRunning(const AbstractApplication &app) const { return m_provider->isRunning(app); }

std::unique_ptr<AbstractAppRuntime> AppRuntime::createProvider(WindowManager &wm) {
#ifdef Q_OS_MACOS
  (void)wm;
  return std::make_unique<MacAppRuntime>();
#else
  return std::make_unique<LinuxAppRuntime>(wm);
#endif
}
