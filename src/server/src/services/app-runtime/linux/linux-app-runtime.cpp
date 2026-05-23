#include "linux-app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"

LinuxAppRuntime::LinuxAppRuntime(WindowManager &wm, AppService &appService)
    : m_wm(wm), m_appService(appService) {
  connect(&m_wm, &WindowManager::windowsChanged, this, &LinuxAppRuntime::runningAppsChanged);
  connect(&m_wm, &WindowManager::focusChanged, this, &LinuxAppRuntime::frontmostAppChanged);
}

bool LinuxAppRuntime::isRunning(const AbstractApplication &app) const {
  return !m_wm.findAppWindows(app).empty();
}

std::shared_ptr<AbstractApplication> LinuxAppRuntime::frontmostApp() const {
  auto focused = m_wm.getFocusedWindow();
  if (!focused) return nullptr;
  return m_appService.findByClass(focused->wmClass());
}

bool LinuxAppRuntime::activate(const AbstractApplication &app) const {
  auto wins = m_wm.findAppWindows(app);
  if (wins.empty()) return false;
  m_wm.provider()->focusWindowSync(*wins.front());
  return true;
}
