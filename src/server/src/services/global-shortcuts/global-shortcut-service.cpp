#include "services/global-shortcuts/global-shortcut-service.hpp"
#include "common/types.hpp"
#include "config/config.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <utility>

GlobalShortcutService::GlobalShortcutService(config::Manager &config, RootItemManager &rootItemManager,
                                             std::unique_ptr<AbstractGlobalShortcutBackend> backend)
    : m_config(config), m_rootItemManager(rootItemManager), m_backend(std::move(backend)) {
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::shortcutActivated, this,
          &GlobalShortcutService::onActivated);
  connect(m_backend.get(), &AbstractGlobalShortcutBackend::ready, this, &GlobalShortcutService::reconcile);
  connect(&m_config, &config::Manager::configChanged, this, [this] { reconcile(); });

  m_backend->start();
  reconcile();
}

void GlobalShortcutService::setCapturing(bool capturing) {
  if (m_capturing == capturing) { return; }
  m_capturing = capturing;

  if (capturing) {
    m_backend->unbindAll();
    m_appliedTriggers.clear();
    m_actions.clear();
  } else {
    reconcile();
  }
}

void GlobalShortcutService::reconcile() {
  if (m_capturing) { return; }
  if (!isSupported()) { return; }

  const config::ConfigValue &cfg = m_config.value();

  std::unordered_map<QString, Desired> desired;

  if (cfg.globalShortcuts.toggle && !cfg.globalShortcuts.toggle->empty()) {
    desired.emplace(QString::fromUtf8(TOGGLE_ID),
                    Desired{.trigger = QString::fromStdString(*cfg.globalShortcuts.toggle),
                            .description = tr("Toggle Vicinae"),
                            .action = ToggleLauncherWindow{}});
  }

  for (const auto &[provider, providerData] : cfg.providers) {
    for (const auto &[entrypoint, item] : providerData.entrypoints) {
      if (!item.shortcut || item.shortcut->empty()) { continue; }
      if (item.enabled.has_value() && !*item.enabled) { continue; }

      EntrypointId eid{provider, entrypoint};
      desired.emplace(QString::fromStdString(eid), Desired{.trigger = QString::fromStdString(*item.shortcut),
                                                           .description = describeCommand(eid),
                                                           .action = RunCommand{eid}});
    }
  }

  for (auto it = m_appliedTriggers.begin(); it != m_appliedTriggers.end();) {
    auto desiredIt = desired.find(it->first);
    if (desiredIt == desired.end() || desiredIt->second.trigger != it->second) {
      m_backend->unbindShortcut(it->first);
      m_actions.erase(it->first);
      it = m_appliedTriggers.erase(it);
    } else {
      ++it;
    }
  }

  for (const auto &[id, entry] : desired) {
    if (auto it = m_appliedTriggers.find(id); it != m_appliedTriggers.end() && it->second == entry.trigger) {
      continue;
    }

    auto shortcut = Keyboard::Shortcut::fromString(entry.trigger);

    if (!shortcut.isValid()) { continue; }

    auto bound = m_backend->bindShortcut({.id = id, .trigger = shortcut, .description = entry.description});
    m_appliedTriggers[id] = entry.trigger;

    if (bound) {
      m_actions[id] = entry.action;
    } else {
      m_actions.erase(id);
      qWarning() << "Failed to bind global shortcut" << id << "(" << entry.trigger << "):" << bound.error();
    }
  }
}

std::optional<QString> GlobalShortcutService::probeBind(const Keyboard::Shortcut &shortcut) {
  if (!isSupported() || !m_capturing) { return std::nullopt; }

  const QString probeId = QStringLiteral("@probe");
  auto bound =
      m_backend->bindShortcut({.id = probeId, .trigger = shortcut, .description = QStringLiteral("Vicinae")});
  m_backend->unbindShortcut(probeId);

  if (!bound) { return bound.error(); }
  return std::nullopt;
}

QString GlobalShortcutService::describeCommand(const EntrypointId &id) const {
  if (auto meta = m_rootItemManager.itemMetadata(id); meta.item) { return meta.item->title(); }
  return QString::fromStdString(id);
}

void GlobalShortcutService::onActivated(const QString &id, quint64 timestamp) {
  if (auto it = m_actions.find(id); it != m_actions.end()) {
    match(
        it->second, [&](const RunCommand &cmd) { emit commandActivated(cmd.id, timestamp); },
        [&](const ToggleLauncherWindow &launcher) { emit toggleLauncherRequested(timestamp); });
  }
}

std::optional<QString> GlobalShortcutService::findConflict(const Keyboard::Shortcut &shortcut,
                                                           const QString &excludeId) const {
  if (!isSupported()) { return std::nullopt; }

  const config::ConfigValue &cfg = m_config.value();
  const auto matches = [&](const std::string &trigger) {
    return Keyboard::Shortcut::fromString(QString::fromStdString(trigger)) == shortcut;
  };

  if (excludeId != QString::fromUtf8(TOGGLE_ID) && cfg.globalShortcuts.toggle &&
      !cfg.globalShortcuts.toggle->empty() && matches(*cfg.globalShortcuts.toggle)) {
    return tr("the launcher hotkey");
  }

  for (const auto &[provider, providerData] : cfg.providers) {
    for (const auto &[entrypoint, item] : providerData.entrypoints) {
      if (!item.shortcut || item.shortcut->empty()) { continue; }

      EntrypointId eid{provider, entrypoint};
      if (QString::fromStdString(eid) == excludeId || !matches(*item.shortcut)) { continue; }

      if (auto meta = m_rootItemManager.itemMetadata(eid); meta.item) { return meta.item->title(); }
      return tr("another command");
    }
  }

  return std::nullopt;
}
