#include "settings-sidebar-model.hpp"
#include "common/entrypoint.hpp"
#include "extension-settings-model.hpp"
#include "fuzzy/fuzzy-searchable.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "view-utils.hpp"
#include "config/config.hpp"
#include <algorithm>
#include <array>

namespace {
const QString kCore = QStringLiteral("core");
const QString kGroup = QStringLiteral("group");
const QString kExt = QStringLiteral("ext");
const QString kCommand = QStringLiteral("command");
const QString kDivider = QStringLiteral("divider");
} // namespace

SettingsSidebarModel::SettingsSidebarModel(ExtensionSettingsModel *extModel, QObject *parent)
    : QAbstractListModel(parent) {
  // We deliberately don't watch RootItemManager::itemsChanged: rebuilding on it
  // would reset the model (and the sidebar's scroll position) out from under the
  // user. rebuildRows() reads the manager live, so each search is fresh and the
  // idle list is simply stale until the next rebuild.
  if (extModel) {
    connect(extModel, &ExtensionSettingsModel::providerEnabledChanged, this,
            &SettingsSidebarModel::setProviderEnabled);
  }
  rebuildRows();
}

int SettingsSidebarModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_rows.size());
}

QVariant SettingsSidebarModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_rows.size())) return {};

  const auto &row = m_rows[index.row()];
  switch (role) {
  case KeyRole:
    return row.key;
  case KindRole:
    return row.kind;
  case LabelRole:
    return row.label;
  case IconRole:
    return row.icon;
  case IconSourceRole:
    return row.iconSource;
  case EnabledRole:
    return row.enabled;
  default:
    return {};
  }
}

QHash<int, QByteArray> SettingsSidebarModel::roleNames() const {
  return {
      {KeyRole, "key"},
      {KindRole, "kind"},
      {LabelRole, "label"},
      {IconRole, "icon"},
      {IconSourceRole, "iconSource"},
      {EnabledRole, "enabled"},
  };
}

void SettingsSidebarModel::setQuery(const QString &query) {
  auto next = query.toStdString();
  if (next == m_query) return;
  m_query = std::move(next);
  beginResetModel();
  rebuildRows();
  endResetModel();
}

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
void SettingsSidebarModel::rebuildRows() {
  m_rows.clear();

  struct CorePage {
    QString id;
    QString label;
    QString icon;
  };
  static const std::array<CorePage, 6> corePages = {{
      {QStringLiteral("general"), QStringLiteral("General"), QStringLiteral("cog")},
      {QStringLiteral("appearance"), QStringLiteral("Appearance"), QStringLiteral("swatch")},
      {QStringLiteral("keybindings"), QStringLiteral("Keybindings"), QStringLiteral("keyboard")},
      {QStringLiteral("ai"), QStringLiteral("AI"), QStringLiteral("stars")},
      {QStringLiteral("advanced"), QStringLiteral("Advanced"), QStringLiteral("wrench-screwdriver")},
      {QStringLiteral("about"), QStringLiteral("About"), QStringLiteral("vicinae")},
  }};

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  auto &cfg = ServiceRegistry::instance()->config()->value();

  auto coreRow = [](const CorePage &p) {
    return Row{.key = p.id, .kind = kCore, .label = p.label, .icon = p.icon};
  };
  auto providerEnabled = [&](const QString &id) {
    if (auto it = cfg.providers.find(id.toStdString()); it != cfg.providers.end())
      return it->second.enabled.value_or(true);
    return true;
  };
  auto providerRow = [&](RootProvider *p) {
    return Row{.key = p->uniqueId(),
               .kind = p->isGroup() ? kGroup : kExt,
               .label = p->displayName(),
               .iconSource = qml::imageSourceFor(p->icon()),
               .enabled = providerEnabled(p->uniqueId())};
  };

  if (m_query.empty()) {
    std::vector<RootProvider *> groups;
    std::vector<RootProvider *> exts;
    for (auto *p : manager->providers()) {
      if (p->isTransient()) continue;
      (p->isGroup() ? groups : exts).push_back(p);
    }

    for (const auto &page : corePages) {
      m_rows.push_back(coreRow(page));
    }
    if (!groups.empty() || !exts.empty()) m_rows.push_back({.kind = kDivider});
    for (auto *p : groups) {
      m_rows.push_back(providerRow(p));
    }
    if (!groups.empty() && !exts.empty()) m_rows.push_back({.kind = kDivider});
    for (auto *p : exts) {
      m_rows.push_back(providerRow(p));
    }
    return;
  }

  struct Top {
    double score;
    Row header;
    std::vector<Row> commands;
  };
  std::vector<Top> tops;

  for (const auto &page : corePages) {
    int const score = fuzzy::scoreWeighted({{page.label.toStdString(), 1.0}}, m_query);
    if (score > 0) tops.push_back({static_cast<double>(score), coreRow(page), {}});
  }

  RootItemPrefixSearchOptions opts;
  opts.includeDisabled = true;
  for (auto &group : manager->searchGroupedByProvider(QString::fromStdString(m_query), opts)) {
    std::vector<Row> commands;
    commands.reserve(group.items.size());
    for (const auto &gi : group.items) {
      auto id = gi.item->uniqueId();
      commands.push_back({.key = QString::fromStdString(std::string(id)),
                          .kind = kCommand,
                          .label = gi.item->title(),
                          .iconSource = qml::imageSourceFor(gi.item->iconUrl()),
                          .enabled = gi.enabled});
    }
    tops.push_back({group.score, providerRow(group.provider), std::move(commands)});
  }

  std::stable_sort(tops.begin(), tops.end(), [](const auto &a, const auto &b) { return a.score > b.score; });

  for (auto &top : tops) {
    m_rows.push_back(std::move(top.header));
    for (auto &c : top.commands) {
      m_rows.push_back(std::move(c));
    }
  }
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

void SettingsSidebarModel::setProviderEnabled(const QString &providerId, bool enabled) {
  // Config is the source of truth (a later rebuild re-reads it); here we just
  // reflect the change on the currently visible provider row.
  for (int i = 0; i < static_cast<int>(m_rows.size()); ++i) {
    auto &row = m_rows[i];
    if (row.key == providerId && row.kind != kCommand && row.kind != kDivider) {
      if (row.enabled != enabled) {
        row.enabled = enabled;
        auto idx = index(i);
        emit dataChanged(idx, idx, {EnabledRole});
      }
      break;
    }
  }
}

int SettingsSidebarModel::indexOfKey(const QString &key) const {
  for (int i = 0; i < static_cast<int>(m_rows.size()); ++i) {
    if (m_rows[i].kind != kDivider && m_rows[i].key == key) return i;
  }
  return -1;
}

QString SettingsSidebarModel::keyAt(int row) const {
  return (row >= 0 && row < static_cast<int>(m_rows.size())) ? m_rows[row].key : QString();
}

QString SettingsSidebarModel::kindAt(int row) const {
  return (row >= 0 && row < static_cast<int>(m_rows.size())) ? m_rows[row].kind : QString();
}

int SettingsSidebarModel::firstSelectableRow() const {
  for (int i = 0; i < static_cast<int>(m_rows.size()); ++i) {
    if (m_rows[i].kind != kDivider) return i;
  }
  return -1;
}

int SettingsSidebarModel::lastSelectableRow() const {
  for (int i = static_cast<int>(m_rows.size()) - 1; i >= 0; --i) {
    if (m_rows[i].kind != kDivider) return i;
  }
  return -1;
}

int SettingsSidebarModel::stepRow(int fromRow, int delta) const {
  if (m_rows.empty()) return -1;
  if (fromRow < 0) return delta > 0 ? firstSelectableRow() : lastSelectableRow();

  int i = fromRow + delta;
  while (i >= 0 && i < static_cast<int>(m_rows.size())) {
    if (m_rows[i].kind != kDivider) return i;
    i += delta;
  }
  return fromRow;
}
