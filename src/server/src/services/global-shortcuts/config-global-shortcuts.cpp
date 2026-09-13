#include "services/global-shortcuts/config-global-shortcuts.hpp"
#include <utility>
#include "config/config.hpp"
#include "navigation-controller.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

ConfigGlobalShortcuts::ConfigGlobalShortcuts(GlobalShortcutService &service, config::Manager &config,
                                             RootItemManager &rootItemManager,
                                             NavigationController &navigation)
    : m_service(service), m_config(config), m_rootItemManager(rootItemManager), m_navigation(navigation) {
  connect(&m_config, &config::Manager::configChanged, this, &ConfigGlobalShortcuts::apply);
  apply();
}

void ConfigGlobalShortcuts::apply() {
  const config::ConfigValue &cfg = m_config.value();

  std::vector<GlobalShortcutHandle> next;
  next.reserve(m_handles.size() + 1);

  if (cfg.globalShortcuts.toggle && !cfg.globalShortcuts.toggle->empty()) {
    const auto id = QString::fromUtf8(TOGGLE_ID);
    next.emplace_back(m_service.bind(id, {.trigger = Keyboard::Shortcut::fromString(
                                              QString::fromStdString(*cfg.globalShortcuts.toggle)),
                                          .description = tr("Toggle Vicinae"),
                                          .onActivated = [this] { m_navigation.toggleWindow(); }}));
  }

  for (const auto &[provider, providerData] : cfg.providers) {
    for (const auto &[entrypoint, item] : providerData.entrypoints) {
      if (!item.shortcut || item.shortcut->empty()) { continue; }
      if (item.enabled.has_value() && !*item.enabled) { continue; }

      EntrypointId eid{provider, entrypoint};
      const auto id = QString::fromStdString(eid);
      next.emplace_back(m_service.bind(
          id, {.trigger = Keyboard::Shortcut::fromString(QString::fromStdString(*item.shortcut)),
               .description = describeCommand(eid),
               .onActivated = [this, eid] { m_navigation.activateEntrypoint(eid); },
               .onReleased = [this, eid] { m_navigation.releaseEntrypoint(eid); }}));
    }
  }

  m_handles = std::move(next);
}

QString ConfigGlobalShortcuts::describeCommand(const EntrypointId &id) const {
  if (auto meta = m_rootItemManager.itemMetadata(id); meta.item) { return meta.item->title(); }
  return QString::fromStdString(id);
}
