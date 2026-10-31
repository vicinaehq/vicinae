#include "builtins/raycast/raycast-store-model.hpp"
#include "actions/extension-actions.hpp"
#include "builtins/raycast/raycast-store-detail-host.hpp"
#include "internal/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"
#include "ui/views/view-utils.hpp"
#include <QCoreApplication>

void RaycastStoreSection::setEntries(const std::vector<Raycast::Extension> &extensions,
                                     ExtensionRegistry *registry, const Raycast::CompatMap &compat,
                                     const QString &sectionName) {
  m_entries.clear();
  m_entries.reserve(extensions.size());
  for (const auto &ext : extensions) {
    std::optional<Raycast::CompatTier> tier;
    if constexpr (Raycast::hasCompatSheet()) {
      tier = Raycast::CompatTier::Unknown;
      if (auto it = compat.find(ext.name.toStdString()); it != compat.end()) {
        tier = Raycast::compatTierFromInfo(it->second);
      }
    }
    m_entries.push_back({.extension = ext, .installed = registry->isInstalled(ext.id), .compatTier = tier});
  }
  m_sectionName = sectionName;
  notifyChanged();
}

QString RaycastStoreSection::itemTitle(int i) const { return m_entries[i].extension.title; }

QString RaycastStoreSection::itemSubtitle(int i) const { return m_entries[i].extension.description; }

std::optional<ImageURL> RaycastStoreSection::itemIcon(int i) const {
  return m_entries[i].extension.themedIcon();
}

std::unique_ptr<ActionPanelState> RaycastStoreSection::actionPanel(int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  auto danger = panel->createSection();

  auto showDetails = new StaticAction(
      QCoreApplication::translate("RaycastStoreSection", "Show details"),
      ImageURL::builtin(BuiltinIcon::ComputerChip),
      [ext = entry.extension, scope = this->scope()]() { scope.pushView(new RaycastStoreDetailHost(ext)); });

  showDetails->setShortcut(Keyboard::Shortcut::enter());

  panel->setTitle(entry.extension.name);
  section->addAction(showDetails);

  if (entry.installed) {
    auto uninstall = new UninstallExtensionAction(entry.extension.id);
    uninstall->setShortcut(Keybind::RemoveAction);
    danger->addAction(uninstall);
  } else {
    section->addAction(new InstallExtensionAction(
        QString("store.raycast.%1").arg(entry.extension.name), entry.extension.themedIcon(),
        [url = entry.extension.download_url](const ApplicationContext *ctx) {
          return ctx->services->raycastStore()->downloadExtension(url);
        }));
  }

  showDetails->setPrimary(true);

  return panel;
}

QVariant RaycastStoreSection::customData(int i, int role) const {
  const auto &entry = m_entries[i];
  switch (role) {
  case DownloadCount:
    return formatCount(entry.extension.download_count);
  case AuthorAvatar:
    return imageSourceFor(entry.extension.author.validUserIcon());
  case IsInstalled:
    return entry.installed;
  case CompatTierRole:
    return entry.compatTier ? static_cast<int>(*entry.compatTier) : -1;
  default:
    return {};
  }
}
