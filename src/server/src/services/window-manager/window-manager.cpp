#include "window-manager.hpp"
#include <algorithm>
#include <qnamespace.h>
#include <ranges>
#include "dummy-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"
#ifdef Q_OS_LINUX
#include "hyprland/hyprland.hpp"
#include "gnome/gnome-window-manager.hpp"
#include "services/window-manager/kde/kde-window-manager.hpp"
#include "niri/niri.hpp"
#include "x11/x11-window-manager.hpp"
#include "wayland/wayland.hpp"
#endif
#ifdef Q_OS_MACOS
#include "macos/macos-window-manager.hpp"
#endif
#ifdef Q_OS_WIN
#include "windows/windows-window-manager.hpp"
#endif

std::vector<std::unique_ptr<AbstractWindowManager>> WindowManager::createCandidates() {
  // XXX - For all new window managers, it is needed to add it to this vector
  std::vector<std::unique_ptr<AbstractWindowManager>> candidates;

#ifdef Q_OS_LINUX
  candidates.emplace_back(std::make_unique<HyprlandWindowManager>());
  candidates.emplace_back(std::make_unique<GnomeWindowManager>());
  candidates.emplace_back(std::make_unique<KDE::WindowManager>());
  candidates.emplace_back(std::make_unique<X11WindowManager>());
  candidates.emplace_back(std::make_unique<Niri::WindowManager>());

  // this implementation is good enough for most standalone wayland compositors
  candidates.emplace_back(std::make_unique<WaylandWindowManager>());
#endif

#ifdef Q_OS_MACOS
  candidates.emplace_back(std::make_unique<MacosWindowManager>());
#endif

#ifdef Q_OS_WIN
  candidates.emplace_back(std::make_unique<Win::WindowManager>());
#endif

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

const AbstractWindowManager::WindowList &WindowManager::listWindows() const { return m_windows; }

AbstractWindowManager::WorkspacePtr WindowManager::findWorkspaceById(const QString &id) {
  if (!m_workspaces) {
    m_workspaces =
        m_provider->hasWorkspaces() ? m_provider->listWorkspaces() : AbstractWindowManager::WorkspaceList{};
  }

  auto pred = [&](auto &&ws) { return ws->id() == id; };
  if (auto it = std::ranges::find_if(*m_workspaces, pred); it != m_workspaces->end()) { return *it; }
  return nullptr;
}

AbstractWindowManager::WindowList WindowManager::findAppWindows(const AbstractApplication &app) const {
  return m_windows | std::views::filter([&](auto &&win) {
           return app.matchesWindowClass(win->wmClass()) ||
                  app.displayName().compare(win->title(), Qt::CaseInsensitive) == 0;
         }) |
         std::ranges::to<std::vector>();
}

void WindowManager::updateWindowCache() {
  m_windows = m_provider->listWindowsSync();
  m_workspaces.reset();
}

bool WindowManager::isCapable() const { return m_provider->id() != "dummy"; }

WindowManager::WindowManager() {
  m_provider = createProvider();
  updateWindowCache();

  connect(m_provider.get(), &AbstractWindowManager::windowsChanged, this, [this]() {
    updateWindowCache();
    emit windowsChanged();
  });

  connect(m_provider.get(), &AbstractWindowManager::focusChanged, this, &WindowManager::focusChanged);
}
