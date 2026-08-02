#pragma once
#include <atomic>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <QObject>
#include "common/entrypoint.hpp"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

namespace config {
class Manager;
}

class AppService;
class RootItemManager;
class WindowManager;

/**
 * Projects the global shortcuts declared in the config onto the provided backend, and dispatches
 * activations to actions. Config is the single source of truth: there is no separate storage.
 * Per-command shortcuts live in `providers[*].entrypoints[*].shortcut`, app-level ones (the
 * launcher toggle, ...) in `globalShortcuts`. The internal `keybinds` map is NOT global and is
 * deliberately ignored here.
 */
class GlobalShortcutService : public QObject {
  Q_OBJECT

signals:
  void toggleLauncherRequested(quint64 timestamp);
  void commandActivated(const EntrypointId &id, quint64 timestamp);

public:
  // Non-command global shortcuts are prefixed with '@' to avoid colliding with entrypoint ids.
  static constexpr const char *TOGGLE_ID = "@toggle-launcher";

  GlobalShortcutService(config::Manager &config, RootItemManager &rootItemManager,
                        WindowManager &windowManager, AppService &appService,
                        std::unique_ptr<AbstractGlobalShortcutBackend> backend);

  AbstractGlobalShortcutBackend *backend() const { return m_backend.get(); }
  bool isSupported() const { return m_backend && m_backend->isSupported(); }

  // Display name of the global shortcut already using `shortcut` (excluding `excludeId`), or nullopt.
  // Always nullopt when the backend is unsupported (inert shortcuts can't actually conflict).
  std::optional<QString> findConflict(const Keyboard::Shortcut &shortcut, const QString &excludeId) const;

  std::optional<QString> probeBind(const Keyboard::Shortcut &shortcut);

  // Suspends all global binds while the in-app recorder captures (so it doesn't hijack the keystroke),
  // then rebinds from config on release.
  void setCapturing(bool capturing);

private:
  struct RunCommand {
    EntrypointId id;
  };

  struct ToggleLauncherWindow {};

  using Action = std::variant<RunCommand, ToggleLauncherWindow>;

  struct Desired {
    QString trigger;
    QString description;
    Action action;
  };

  void reconcile();
  void onActivated(const QString &id, quint64 timestamp);
  QString describeCommand(const EntrypointId &id) const;

  // Recomputes whether the toggle shortcut should be suppressed for the currently focused app.
  // Cheap to call often: the actual window/app lookup only happens here, never on the hot path of
  // a keypress. Consulted by `m_backend` via the activation gate, possibly from a non-Qt thread.
  void updateToggleSuppression();

  config::Manager &m_config;
  RootItemManager &m_rootItemManager;
  WindowManager &m_windowManager;
  AppService &m_appService;
  std::unordered_map<QString, Action> m_actions;
  std::unordered_map<QString, QString> m_appliedTriggers;
  std::unordered_set<std::string> m_hotkeyExcludedAppIds;
  std::atomic<bool> m_toggleSuppressed{false};
  bool m_capturing = false;
  std::unique_ptr<AbstractGlobalShortcutBackend> m_backend;
};
