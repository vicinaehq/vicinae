#include "vicinae-store-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "vicinae-store-detail-host.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"

VicinaeStoreModel::VicinaeStoreModel(QObject *parent) : CommandListModel(parent) {}

void VicinaeStoreModel::setEntries(const std::vector<VicinaeStore::Extension> &extensions,
                                      ExtensionRegistry *registry, const QString &sectionName) {
  m_entries.clear();
  m_entries.reserve(extensions.size());
  for (const auto &ext : extensions) {
    m_entries.push_back({.extension = ext, .installed = registry->isInstalled(ext.id)});
  }

  std::vector<SectionInfo> sections;
  if (!m_entries.empty()) {
    sections.push_back({.name = sectionName, .count = static_cast<int>(m_entries.size())});
  }
  setSections(sections);
}

QHash<int, QByteArray> VicinaeStoreModel::roleNames() const {
  auto roles = CommandListModel::roleNames();
  roles[DownloadCount] = "downloadCount";
  roles[AuthorAvatar] = "authorAvatar";
  roles[IsInstalled] = "isInstalled";
  return roles;
}

QVariant VicinaeStoreModel::data(const QModelIndex &index, int role) const {
  int s, i;
  if (role >= DownloadCount && !dataItemAt(index.row(), s, i)) return {};

  switch (role) {
  case DownloadCount:
    return formatCount(m_entries[i].extension.downloadCount);
  case AuthorAvatar: {
    const auto &avatar = m_entries[i].extension.author.avatarUrl;
    if (avatar.isEmpty()) return imageSourceFor(ImageURL::builtin("person"));
    return QStringLiteral("image://vicinae/http:%1?mask=circle").arg(avatar);
  }
  case IsInstalled:
    return m_entries[i].installed;
  default:
    return CommandListModel::data(index, role);
  }
}

QString VicinaeStoreModel::itemTitle(int, int i) const { return m_entries[i].extension.title; }

QString VicinaeStoreModel::itemSubtitle(int, int i) const {
  return m_entries[i].extension.description;
}

QString VicinaeStoreModel::itemIconSource(int, int i) const {
  return imageSourceFor(ImageURL::http(m_entries[i].extension.themedIcon()));
}

std::unique_ptr<ActionPanelState> VicinaeStoreModel::createActionPanel(int, int i) const {
  const auto &entry = m_entries[i];
  auto panel = std::make_unique<ActionPanelState>();
  auto section = panel->createSection();
  auto danger = panel->createSection();

  auto showDetails = new StaticAction(
      "Show details", ImageURL::builtin("computer-chip"),
      [ext = entry.extension, scope = this->scope()]() {
        scope.pushView(new VicinaeStoreDetailHost(ext));
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
