#include "window-manager.hpp"
#include "hyprland/hyprland.hpp"
#include "dummy-window-manager.hpp"
#include "services/window-manager/abstract-window-manager.hpp"

std::vector<std::unique_ptr<AbstractWindowManager>> WindowManager::createCandidates() {
  // XXX - For all new window managers, it is needed to add it to this vector
  std::vector<std::unique_ptr<AbstractWindowManager>> candidates;

  candidates.emplace_back(std::make_unique<HyprlandWindowManager>());

  return candidates;
}

std::unique_ptr<AbstractWindowManager> WindowManager::createProvider() {
  for (auto &candidate : createCandidates()) {
    if (candidate->isActivatable()) { return std::move(candidate); }
  }

  return std::make_unique<DummyWindowManager>();
}

AbstractWindowManager *WindowManager::provider() const { return m_provider.get(); }

AbstractWindowManager::WindowList WindowManager::listWindowsSync() { return m_provider->listWindowsSync(); }

AbstractWindowManager::WindowPtr WindowManager::getFocusedWindow() {
  return m_provider->getFocusedWindowSync();
}

bool WindowManager::canPaste() const { return m_provider->supportsInputForwarding(); }

WindowManager::WindowManager() { m_provider = createProvider(); }
