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

  // candidates.emplace_back(std::make_unique<HyprlandWindowManager>());
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
  // Delegate to GNOME-specific implementation if we're using GNOME window manager
  if (auto gnomeWm = dynamic_cast<const GnomeWindowManager *>(m_provider.get())) {
    return gnomeWm->findWindowByClassGnome(wmClass);
  }

  AbstractWindowManager::WindowList filtered;

  for (const auto &win : m_windows) {
    if (win->wmClass() == wmClass) { filtered.emplace_back(win); }
  }

  return filtered;
}

AbstractWindowManager::WindowList WindowManager::findAppWindows(const AbstractApplication &app) const {
  // Delegate to GNOME-specific implementation if we're using GNOME window manager
  if (auto gnomeWm = dynamic_cast<const GnomeWindowManager *>(m_provider.get())) {
    return gnomeWm->findAppWindowsGnome(app);
  }

  auto pred = [&](auto &&win) {
    return std::ranges::any_of(app.windowClasses(),
                               [&](auto &&s) { return s.toLower() == win->wmClass().toLower(); });
  };
  AbstractWindowManager::WindowList filtered;

  for (const auto &win : m_windows | std::views::filter(pred)) {
    filtered.emplace_back(win);
  }

  return filtered;
}

void WindowManager::updateWindowCache() { m_windows = m_provider->listWindowsSync(); }

bool WindowManager::canPaste() const { return m_provider->supportsInputForwarding(); }

WindowManager::WindowManager() {
  m_provider = createProvider();
  updateWindowCache();

  connect(m_provider.get(), &AbstractWindowManager::windowsChanged, this, &WindowManager::updateWindowCache);
}
