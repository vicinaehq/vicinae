#include "window-manager.hpp"
#include <ranges>
#include "hyprland/hyprland.hpp"
#include "gnome/gnome-window-manager.hpp"
#include "dummy-window-manager.hpp"
#include "wayland/wayland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

std::vector<std::unique_ptr<AbstractWindowManager>> WindowManager::createCandidates() {
  // XXX - For all new window managers, it is needed to add it to this vector
  std::vector<std::unique_ptr<AbstractWindowManager>> candidates;

  candidates.emplace_back(std::make_unique<HyprlandWindowManager>());
  candidates.emplace_back(std::make_unique<GnomeWindowManager>());
  candidates.emplace_back(std::make_unique<WaylandWindowManager>());

  return candidates;
}

std::unique_ptr<AbstractWindowManager> WindowManager::createProvider() {
  for (auto &candidate : createCandidates()) {
    if (candidate->isActivatable()) {
      candidate->start();
      return std::move(candidate);
    }
  }

  return std::make_unique<DummyWindowManager>();
}

AbstractWindowManager *WindowManager::provider() const { return m_provider.get(); }

AbstractWindowManager::WindowList WindowManager::listWindowsSync() { return m_provider->listWindowsSync(); }

AbstractWindowManager::WindowPtr WindowManager::getFocusedWindow() {
  return m_provider->getFocusedWindowSync();
}

AbstractWindowManager::WindowList WindowManager::listWindows() const { return m_windows; }

AbstractWindowManager::WindowList WindowManager::findWindowByClass(const QString &wmClass) const {
  auto pred = [&](auto &&win) { return win->wmClass() == wmClass; };

  return listWindows() | std::views::filter(pred) | std::ranges::to<std::vector>();
}

AbstractWindowManager::WindowList WindowManager::findAppWindows(const Application &app) const {
  QString wmClass = app.windowClass().toLower();

  auto pred = [&](auto &&win) { return win->wmClass().toLower() == wmClass; };

  return listWindows() | std::views::filter(pred) | std::ranges::to<std::vector>();
}

void WindowManager::updateWindowCache() { m_windows = m_provider->listWindowsSync(); }

bool WindowManager::canPaste() const { return m_provider->supportsInputForwarding(); }

WindowManager::WindowManager() {
  m_provider = createProvider();
  updateWindowCache();

  connect(m_provider.get(), &AbstractWindowManager::windowsChanged, this, &WindowManager::updateWindowCache);
}
