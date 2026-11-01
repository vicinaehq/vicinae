#include "app-runtime.hpp"
#include <QGuiApplication>
#include "services/window-manager/abstract-window-manager.hpp"
#include "services/window-manager/window-manager.hpp"

#ifdef Q_OS_MACOS
#include "services/app-runtime/macos/mac-app-runtime.hpp"
#elif defined(Q_OS_WIN)
#include "services/app-runtime/windows/win-app-runtime.hpp"
#else
#include "services/app-runtime/linux/linux-app-runtime.hpp"
#endif

AppRuntime::AppRuntime(WindowManager &wm, AppService &appService)
    : m_wm(wm), m_provider(createProvider(wm, appService)) {
  connect(m_provider.get(), &AbstractAppRuntime::runningAppsChanged, this, &AppRuntime::runningAppsChanged);
  connect(m_provider.get(), &AbstractAppRuntime::frontmostAppChanged, this, &AppRuntime::frontmostAppChanged);
}

bool AppRuntime::isRunning(const AbstractApplication &app) const { return m_provider->isRunning(app); }

std::shared_ptr<AbstractApplication> AppRuntime::frontmostApp() const { return m_provider->frontmostApp(); }

std::shared_ptr<AbstractApplication> AppRuntime::focusTargetApp() const {
  // we have focus, and we are a layer so it means that the current window might be:
  // - the previously focused window, not invalidated by the layer grab (Hyprland does this)
  // - nullptr as there is technically no window owning focus (a layer is not a window)
  // this logic is here so that we don't mistakingly use stale data when vicinae itself has focus
  // (e.g using ctrl+shift+v because the last window is a terminal, but we are really focusing the
  // vicinae window)
  // FIXME: currently this logic will, if not using layer shell, null the window even if the currently
  // focused window is the vicinae one. The only problem with this is that app filtering won't work on the
  // vicinae window itself. We probably can fix this by just checking whether we are a layer here, but
  // requires pulling config...
  if (QGuiApplication::focusWindow() && !m_wm.provider()->supportsFocusHandoffDetection()) return nullptr;
  return frontmostApp();
}

bool AppRuntime::activate(const AbstractApplication &app) const { return m_provider->activate(app); }

bool AppRuntime::quit(const AbstractApplication &app) const { return m_provider->quit(app); }

bool AppRuntime::forceQuit(const AbstractApplication &app) const { return m_provider->forceQuit(app); }

std::unique_ptr<AbstractAppRuntime> AppRuntime::createProvider(WindowManager &wm, AppService &appService) {
#ifdef Q_OS_MACOS
  (void)wm;
  return std::make_unique<MacAppRuntime>(appService);
#elif defined(Q_OS_WIN)
  return std::make_unique<WindowsAppRuntime>(wm, appService);
#else
  return std::make_unique<LinuxAppRuntime>(wm, appService);
#endif
}
