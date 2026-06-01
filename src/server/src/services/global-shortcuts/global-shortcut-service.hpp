#pragma once
#include <memory>
#include <unordered_map>
#include <QObject>
#include "common/entrypoint.hpp"
#include "services/global-shortcuts/abstract-global-shortcut-backend.hpp"

namespace config {
class Manager;
}

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
  GlobalShortcutService(config::Manager &config, std::unique_ptr<AbstractGlobalShortcutBackend> backend);

  AbstractGlobalShortcutBackend *backend() const { return m_backend.get(); }

private:
  struct Action {
    enum class Kind { ToggleLauncher, RunCommand } kind;
    EntrypointId entrypoint;
  };

  void reconcile();
  void onActivated(const QString &id, quint64 timestamp);

  config::Manager &m_config;
  std::unique_ptr<AbstractGlobalShortcutBackend> m_backend;
  std::unordered_map<QString, Action> m_actions;
  std::unordered_map<QString, QString> m_boundTriggers;
};
