#pragma once
#include <vector>
#include <QObject>
#include "common/entrypoint.hpp"
#include "services/global-shortcuts/global-shortcut-service.hpp"

namespace config {
class Manager;
}

class NavigationController;
class RootItemManager;

/**
 * Projects the global shortcuts declared in the config onto the GlobalShortcutService. Config is the
 * single source of truth: per-command shortcuts live in `providers[*].entrypoints[*].shortcut`,
 * app-level ones (the launcher toggle, ...) in `globalShortcuts`. The internal `keybinds` map is NOT
 * global and is deliberately ignored here.
 */
class ConfigGlobalShortcuts : public QObject {
  Q_OBJECT

public:
  // Non-command global shortcuts are prefixed with '@' to avoid colliding with entrypoint ids.
  static constexpr const char *TOGGLE_ID = "@toggle-launcher";

  ConfigGlobalShortcuts(GlobalShortcutService &service, config::Manager &config,
                        RootItemManager &rootItemManager, NavigationController &navigation);

private:
  void apply();
  QString describeCommand(const EntrypointId &id) const;

  GlobalShortcutService &m_service;
  config::Manager &m_config;
  RootItemManager &m_rootItemManager;
  NavigationController &m_navigation;
  std::vector<GlobalShortcutHandle> m_handles;
};
