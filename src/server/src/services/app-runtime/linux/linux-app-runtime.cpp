#include "linux-app-runtime.hpp"
#include "services/app-service/app-service.hpp"
#include "services/window-manager/window-manager.hpp"
#include <csignal>
#include <unordered_set>

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

bool LinuxAppRuntime::quit(const AbstractApplication &app) const {
  bool closed = false;
  for (const auto &win : m_wm.findAppWindows(app)) {
    if (m_wm.provider()->closeWindow(*win)) closed = true;
  }
  return closed;
}

bool LinuxAppRuntime::forceQuit(const AbstractApplication &app) const {
  auto wins = m_wm.findAppWindows(app);

  std::unordered_set<int> pids;
  AbstractWindowManager::WindowList pidless;
  pidless.reserve(wins.size());

  for (const auto &win : wins) {
    if (auto pid = win->pid(); pid && *pid > 0) {
      pids.emplace(*pid);
    } else {
      pidless.emplace_back(win);
    }
  }

  bool acted = false;
  for (int pid : pids) {
    if (::kill(pid, SIGKILL) == 0) acted = true;
  }
  for (const auto &win : pidless) {
    if (m_wm.provider()->closeWindow(*win)) acted = true;
  }
  return acted;
}
