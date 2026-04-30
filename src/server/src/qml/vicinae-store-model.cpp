#include "vicinae-store-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "vicinae-store-detail-host.hpp"
#include "lib/keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "utils/utils.hpp"

template <> struct fuzzy::FuzzySearchable<VicinaeStoreModel::Entry> {
  static int score(const VicinaeStoreModel::Entry &entry, std::string_view query) {
    auto title = entry.extension.title.toStdString();
    auto author = entry.extension.author.name.toStdString();
    auto desc = entry.extension.description.toStdString();
    return fuzzy::scoreWeighted({{title, 1.0}, {author, 0.5}, {desc, 0.3}}, query);
  }
};

VicinaeStoreModel::VicinaeStoreModel(QObject *parent) : CommandListModel(parent) {}

void VicinaeStoreModel::setEntries(const std::vector<VicinaeStore::Extension> &extensions,
                                   ExtensionRegistry *registry, const QString &sectionName) {
  m_entries.clear();
  m_entries.reserve(extensions.size());
  for (const auto &ext : extensions) {
    m_entries.push_back({.extension = ext, .installed = registry->isInstalled(ext.id)});
  }
  m_sectionName = sectionName;
  applyFilter();
}

void VicinaeStoreModel::setFilter(const QString &text) {
  m_query = text.toStdString();
  applyFilter();
}

void VicinaeStoreModel::applyFilter() {
  fuzzy::fuzzyFilter<Entry>(std::span<const Entry>(m_entries), m_query, m_filtered);

  std::vector<SectionInfo> sections;
  if (!m_filtered.empty()) {
    sections.push_back({.name = m_sectionName, .count = static_cast<int>(m_filtered.size())});
  }
  setSections(sections);
}

const VicinaeStoreModel::Entry &VicinaeStoreModel::resolvedEntry(int i) const {
  return m_entries[m_filtered[i].data];
}

QHash<int, QByteArray> VicinaeStoreModel::roleNames() const {
  auto roles = CommandListModel::roleNames();
  roles[DownloadCount] = "downloadCount";
  roles[AuthorAvatar] = "authorAvatar";
  roles[IsInstalled] = "isInstalled";
  roles[CompatTierRole] = "compatTier";
  return roles;
}

QVariant VicinaeStoreModel::data(const QModelIndex &index, int role) const {
  int s, i;
  if (role >= DownloadCount && !dataItemAt(index.row(), s, i)) return {};

  switch (role) {
  case DownloadCount:
    return formatCount(resolvedEntry(i).extension.downloadCount);
  case AuthorAvatar: {
    const auto &avatar = resolvedEntry(i).extension.author.avatarUrl;
    if (avatar.isEmpty()) return imageSourceFor(ImageURL::builtin("person"));
    return imageSourceFor(ImageURL::http(QUrl(avatar)).circle());
  }
  case IsInstalled:
    return resolvedEntry(i).installed;
  case CompatTierRole:
    return -1;
  default:
    return CommandListModel::data(index, role);
  }
}

QString VicinaeStoreModel::itemTitle(int, int i) const { return resolvedEntry(i).extension.title; }

QString VicinaeStoreModel::itemSubtitle(int, int i) const { return resolvedEntry(i).extension.description; }

QString VicinaeStoreModel::itemIconSource(int, int i) const {
  return imageSourceFor(resolvedEntry(i).extension.themedIcon());
}

std::unique_ptr<ActionPanelState> VicinaeStoreModel::createActionPanel(int, int i) const {
  const auto &entry = resolvedEntry(i);
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
