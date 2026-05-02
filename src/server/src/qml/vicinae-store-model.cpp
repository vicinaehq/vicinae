#include "vicinae-store-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "vicinae-store-detail-host.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"
#include "view-utils.hpp"

void VicinaeStoreSection::setEntries(const std::vector<VicinaeStore::Extension> &extensions,
                                     ExtensionRegistry *registry, const QString &sectionName) {
  m_sectionName = sectionName;
  std::vector<VicinaeStoreEntry> entries;
  entries.reserve(extensions.size());
  for (const auto &ext : extensions) {
    entries.push_back({.extension = ext, .installed = registry->isInstalled(ext.id)});
  }
  setItems(std::move(entries));
}

QString VicinaeStoreSection::displayTitle(const VicinaeStoreEntry &entry) const {
  return entry.extension.title;
}

QString VicinaeStoreSection::displaySubtitle(const VicinaeStoreEntry &entry) const {
  return entry.extension.description;
}

QString VicinaeStoreSection::displayIconSource(const VicinaeStoreEntry &entry) const {
  return imageSourceFor(entry.extension.themedIcon());
}

std::unique_ptr<ActionPanelState>
VicinaeStoreSection::buildActionPanel(const VicinaeStoreEntry &entry) const {
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  auto danger = panel->createSection();

  auto showDetails = new StaticAction(
      "Show details", ImageURL::builtin("computer-chip"),
      [author = entry.extension.author.handle, name = entry.extension.name, scope = this->scope()]() {
        scope.pushView(new VicinaeStoreDetailHost(author, name));
      });
  auto uninstall = new UninstallExtensionAction(entry.extension.id);

  showDetails->setShortcut(Keyboard::Shortcut::enter());
  uninstall->setShortcut(Keybind::RemoveAction);

  panel->setTitle(entry.extension.name);
  section->addAction(showDetails);
  danger->addAction(uninstall);
  showDetails->setPrimary(true);

  return panel;
}

QVariant VicinaeStoreSection::customData(int i, int role) const {
  const auto &entry = at(i);
  switch (role) {
  case DownloadCount:
    return formatCount(entry.extension.downloadCount);
  case AuthorAvatar: {
    const auto &avatar = entry.extension.author.avatarUrl;
    if (avatar.isEmpty()) return imageSourceFor(ImageURL::builtin("person"));
    return imageSourceFor(ImageURL::http(QUrl(avatar)).circle());
  }
  case IsInstalled:
    return entry.installed;
  case CompatTierRole:
    return -1;
  default:
    return {};
  }
}
