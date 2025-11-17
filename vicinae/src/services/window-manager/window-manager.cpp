#include "window-manager.hpp"
#include <algorithm>
#include <ranges>
#include "hyprland/hyprland.hpp"
#include "gnome/gnome-window-manager.hpp"
#include "x11/x11-window-manager.hpp"
#include "dummy-window-manager.hpp"
#include "wayland/wayland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

std::vector<std::unique_ptr<AbstractWindowManager>> WindowManager::createCandidates() {
  // XXX - For all new window managers, it is needed to add it to this vector
  std::vector<std::unique_ptr<AbstractWindowManager>> candidates;

  candidates.emplace_back(std::make_unique<HyprlandWindowManager>());
  candidates.emplace_back(std::make_unique<GnomeWindowManager>());
  candidates.emplace_back(std::make_unique<X11WindowManager>());

  // this implementation is good enough for most standalone wayland compositors
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

const AbstractWindowManager::AbstractWindow *WindowManager::findWindowById(const QString &id) {
  auto pred = [&](auto &&win) { return win->id() == id; };
  if (auto it = std::ranges::find_if(m_windows, pred); it != m_windows.end()) { return it->get(); }
  return nullptr;
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

bool WindowManager::focusApp(const AbstractApplication &app) const {
  if (auto wins = findAppWindows(app); !wins.empty()) {
    provider()->focusWindowSync(*wins.front());
    return true;
  }

  return false;
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

bool WindowManager::canPaste() const { return m_provider->supportsPaste(); }

WindowManager::WindowManager() {
  m_provider = createProvider();
  updateWindowCache();

  connect(m_provider.get(), &AbstractWindowManager::windowsChanged, this, [this]() {
    updateWindowCache();
    emit windowsChanged();
  });
}
