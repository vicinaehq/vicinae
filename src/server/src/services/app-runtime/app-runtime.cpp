#include "app-runtime.hpp"

#ifdef Q_OS_MACOS
#include "services/app-runtime/macos/mac-app-runtime.hpp"
#else
#include "services/app-runtime/linux/linux-app-runtime.hpp"
#endif

AppRuntime::AppRuntime(WindowManager &wm, AppService &appService)
    : m_provider(createProvider(wm, appService)) {
  connect(m_provider.get(), &AbstractAppRuntime::runningAppsChanged, this, &AppRuntime::runningAppsChanged);
  connect(m_provider.get(), &AbstractAppRuntime::frontmostAppChanged, this, &AppRuntime::frontmostAppChanged);
}

bool AppRuntime::isRunning(const AbstractApplication &app) const { return m_provider->isRunning(app); }

std::shared_ptr<AbstractApplication> AppRuntime::frontmostApp() const { return m_provider->frontmostApp(); }

bool AppRuntime::activate(const AbstractApplication &app) const { return m_provider->activate(app); }

bool AppRuntime::quit(const AbstractApplication &app) const { return m_provider->quit(app); }

bool AppRuntime::forceQuit(const AbstractApplication &app) const { return m_provider->forceQuit(app); }

std::unique_ptr<AbstractAppRuntime> AppRuntime::createProvider(WindowManager &wm, AppService &appService) {
#ifdef Q_OS_MACOS
  (void)wm;
  return std::make_unique<MacAppRuntime>(appService);
#else
  return std::make_unique<LinuxAppRuntime>(wm, appService);
#endif
}
