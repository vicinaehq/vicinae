#include "window-manager.hpp"
#include <algorithm>
#include <qnamespace.h>
#include <ranges>
#include "hyprland/hyprland.hpp"
#include "gnome/gnome-window-manager.hpp"
#include "services/window-manager/kde/kde-window-manager.hpp"
#include "x11/x11-window-manager.hpp"
#include "dummy-window-manager.hpp"
#include "wayland/wayland.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

std::vector<std::unique_ptr<AbstractWindowManager>> WindowManager::createCandidates() {
  // XXX - For all new window managers, it is needed to add it to this vector
  std::vector<std::unique_ptr<AbstractWindowManager>> candidates;

  candidates.emplace_back(std::make_unique<HyprlandWindowManager>());
  candidates.emplace_back(std::make_unique<GnomeWindowManager>());
  candidates.emplace_back(std::make_unique<KDE::WindowManager>());
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

bool WindowManager::focusApp(const AbstractApplication &app) const {
  if (auto wins = findAppWindows(app); !wins.empty()) {
    provider()->focusWindowSync(*wins.front());
    return true;
  }

  return false;
}

AbstractWindowManager::WindowList WindowManager::findAppWindows(const AbstractApplication &app) const {
  return m_windows | std::views::filter([&](auto &&win) {
           return app.matchesWindowClass(win->wmClass()) ||
                  app.displayName().compare(win->title(), Qt::CaseInsensitive) == 0;
         }) |
         std::ranges::to<std::vector>();
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
