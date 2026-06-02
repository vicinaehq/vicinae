#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "common/types.hpp"
#include "config/config.hpp"
#include <utility>

namespace {

// non command global shortcuts are always prefix with '@' to prevent potential conflicts
constexpr const auto TOGGLE_ID = "@toggle-launcher";

} // namespace

GlobalShortcutService::GlobalShortcutService(config::Manager &config,
                                             std::unique_ptr<AbstractGlobalShortcutBackend> backend)
    : m_config(config), m_backend(std::move(backend)) {
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::shortcutActivated, this,
          &GlobalShortcutService::onActivated);
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::ready, this, &GlobalShortcutService::reconcile);
  connect(&m_config, &config::Manager::configChanged, this, [this] { reconcile(); });

  m_backend->start();
  reconcile();
}

void GlobalShortcutService::reconcile() {
  const config::ConfigValue &cfg = m_config.value();

  std::unordered_map<QString, std::pair<QString, Action>> desired;

  if (cfg.globalShortcuts.toggle && !cfg.globalShortcuts.toggle->empty()) {
    desired.emplace(TOGGLE_ID, std::pair{*cfg.globalShortcuts.toggle, ToggleLauncherWindow{}});
  }

  for (const auto &[provider, providerData] : cfg.providers) {
    for (const auto &[entrypoint, item] : providerData.entrypoints) {
      if (!item.shortcut || item.shortcut->empty()) { continue; }
      if (item.enabled.has_value() && !*item.enabled) { continue; }

      EntrypointId eid{provider, entrypoint};
      desired.emplace(eid, std::pair{*item.shortcut, RunCommand{eid}});
    }
  }

  for (auto it = m_boundTriggers.begin(); it != m_boundTriggers.end();) {
    auto desiredIt = desired.find(it->first);
    if (desiredIt == desired.end() || desiredIt->second.first != it->second) {
      m_backend->unbindShortcut(it->first);
      m_actions.erase(it->first);
      it = m_boundTriggers.erase(it);
    } else {
      ++it;
    }
  }

  for (const auto &[id, entry] : desired) {
    const auto &[trigger, action] = entry;
    if (auto it = m_boundTriggers.find(id); it != m_boundTriggers.end() && it->second == trigger) {
      continue;
    }

    auto shortcut = Keyboard::Shortcut::fromString(trigger);

    if (!shortcut.isValid()) { continue; }

    m_backend->bindShortcut({.id = id, .description = id, .trigger = shortcut});
    m_actions[id] = action;
    m_boundTriggers[id] = trigger;
  }
}

void GlobalShortcutService::onActivated(const QString &id, quint64 timestamp) {
  if (auto it = m_actions.find(id); it != m_actions.end()) {
    match(
        it->second, [&](const RunCommand &cmd) { emit commandActivated(cmd.id, timestamp); },
        [&](const ToggleLauncherWindow &launcher) { emit toggleLauncherRequested(timestamp); });
  }
}
