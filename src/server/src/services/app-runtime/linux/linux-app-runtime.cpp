#include "linux-app-runtime.hpp"
#include "services/window-manager/window-manager.hpp"

LinuxAppRuntime::LinuxAppRuntime(WindowManager &wm) : m_wm(wm) {
  connect(&m_wm, &WindowManager::windowsChanged, this, &LinuxAppRuntime::runningAppsChanged);
}

bool LinuxAppRuntime::isRunning(const AbstractApplication &app) const {
  return !m_wm.findAppWindows(app).empty();
}
