#include "raycast-store-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "raycast-store-detail-host.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"

void RaycastStoreSection::setEntries(const std::vector<Raycast::Extension> &extensions,
                                     ExtensionRegistry *registry, const Raycast::CompatMap &compat,
                                     const QString &sectionName) {
  m_entries.clear();
  m_entries.reserve(extensions.size());
  for (const auto &ext : extensions) {
    auto tier = Raycast::CompatTier::Unknown;
    if (auto it = compat.find(ext.name.toStdString()); it != compat.end()) {
      tier = Raycast::compatTierFromInfo(it->second);
    }
    m_entries.push_back({.extension = ext, .installed = registry->isInstalled(ext.id), .compatTier = tier});
  }
  m_sectionName = sectionName;
  notifyChanged();
}

QString RaycastStoreSection::itemTitle(int i) const { return m_entries[i].extension.title; }

QString RaycastStoreSection::itemSubtitle(int i) const { return m_entries[i].extension.description; }

QString RaycastStoreSection::itemIconSource(int i) const {
  return imageSourceFor(m_entries[i].extension.themedIcon());
}

std::unique_ptr<ActionPanelState> RaycastStoreSection::actionPanel(int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  auto danger = panel->createSection();

  auto showDetails = new StaticAction(
      "Show details", ImageURL::builtin("computer-chip"),
      [ext = entry.extension, scope = this->scope()]() { scope.pushView(new RaycastStoreDetailHost(ext)); });
  auto uninstall = new UninstallExtensionAction(entry.extension.id);

  showDetails->setShortcut(Keyboard::Shortcut::enter());
  uninstall->setShortcut(Keybind::RemoveAction);

  panel->setTitle(entry.extension.name);
  section->addAction(showDetails);
  danger->addAction(uninstall);
  showDetails->setPrimary(true);

  return panel;
}
