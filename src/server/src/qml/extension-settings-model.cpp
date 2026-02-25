#include "extension-settings-model.hpp"
#include "view-utils.hpp"
#include "root-search/extensions/extension-root-provider.hpp"
#include "service-registry.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include <algorithm>

ExtensionSettingsModel::ExtensionSettingsModel(QObject *parent) : QAbstractListModel(parent) {
  m_prefModel = new PreferenceFormModel(this);
  auto *manager = ServiceRegistry::instance()->rootItemManager();
  connect(manager, &RootItemManager::itemsChanged, this, [this]() { rebuild(m_filter); });
  rebuild({});
}

int ExtensionSettingsModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_visibleIndices.size());
}

QVariant ExtensionSettingsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_visibleIndices.size()))
    return {};
  const auto &e = m_allEntries[m_visibleIndices[index.row()]];
  switch (role) {
  case NameRole:
    return e.name;
  case TypeRole:
    return e.type;
  case IconSourceRole:
    return e.iconSource;
  case IsProviderRole:
    return e.isProvider;
  case IndentRole:
    return e.indent;
  case EnabledRole:
    return e.enabled;
  case AliasRole:
    return e.alias;
  case EntrypointIdRole:
    return e.isProvider ? e.providerId : QString::fromStdString(e.entrypointId);
  case ExpandedRole:
    return e.expanded;
  case ExpandableRole:
    return e.isProvider && e.childCount > 0;
  default:
    return {};
  }
}

QHash<int, QByteArray> ExtensionSettingsModel::roleNames() const {
  return {{NameRole, "name"},
          {TypeRole, "type"},
          {IconSourceRole, "iconSource"},
          {IsProviderRole, "isProvider"},
          {IndentRole, "indent"},
          {EnabledRole, "enabled"},
          {AliasRole, "alias"},
          {EntrypointIdRole, "entrypointId"},
          {ExpandedRole, "expanded"},
          {ExpandableRole, "expandable"}};
}

QString ExtensionSettingsModel::selectedTitle() const {
  if (!hasSelection()) return {};
  return m_allEntries[m_visibleIndices[m_selectedRow]].name;
}

QString ExtensionSettingsModel::selectedIconSource() const {
  if (!hasSelection()) return {};
  return m_allEntries[m_visibleIndices[m_selectedRow]].iconSource;
}

QString ExtensionSettingsModel::selectedDescription() const {
  if (!hasSelection()) return {};
  return m_allEntries[m_visibleIndices[m_selectedRow]].description;
}

QVariantList ExtensionSettingsModel::selectedMetadata() const {
  if (!hasSelection()) return {};
  return m_allEntries[m_visibleIndices[m_selectedRow]].metadata;
}

bool ExtensionSettingsModel::hasSelection() const {
  return m_selectedRow >= 0 && m_selectedRow < static_cast<int>(m_visibleIndices.size());
}

bool ExtensionSettingsModel::hasPreferences() const { return hasSelection() && m_prefModel->rowCount() > 0; }

bool ExtensionSettingsModel::selectedIsProvider() const {
  return hasSelection() && m_allEntries[m_visibleIndices[m_selectedRow]].isProvider;
}

bool ExtensionSettingsModel::selectedEnabled() const {
  return hasSelection() && m_allEntries[m_visibleIndices[m_selectedRow]].enabled;
}

QString ExtensionSettingsModel::selectedAlias() const {
  return hasSelection() ? m_allEntries[m_visibleIndices[m_selectedRow]].alias : QString();
}

void ExtensionSettingsModel::setFilter(const QString &text) {
  if (m_filter == text) return;
  m_filter = text;
  rebuild(text);
}

void ExtensionSettingsModel::select(int row) {
  if (row == m_selectedRow) return;
  m_selectedRow = row;

  if (hasSelection()) {
    auto &e = m_allEntries[m_visibleIndices[m_selectedRow]];
    auto *manager = ServiceRegistry::instance()->rootItemManager();
    if (e.isProvider) {
      auto *provider = manager->provider(e.providerId.toStdString());
      if (provider)
        m_prefModel->loadProvider(e.providerId, provider->preferences());
      else
        m_prefModel->loadProvider(e.providerId, {});
    } else {
      auto *item = manager->findItemById(e.entrypointId);
      m_prefModel->load(e.entrypointId, item ? item->preferences() : std::vector<Preference>{});
    }
  }

  emit selectedChanged();
}

void ExtensionSettingsModel::setEnabled(int row, bool value) {
  if (row < 0 || row >= static_cast<int>(m_visibleIndices.size())) return;
  auto &e = m_allEntries[m_visibleIndices[row]];
  auto *manager = ServiceRegistry::instance()->rootItemManager();

  if (e.isProvider) {
    manager->setProviderEnabled(e.providerId, value);
    e.enabled = value;
    auto idx = index(row);
    emit dataChanged(idx, idx, {EnabledRole});

    int allIdx = m_visibleIndices[row];
    for (int i = allIdx + 1; i < static_cast<int>(m_allEntries.size()) && !m_allEntries[i].isProvider; ++i) {
      m_allEntries[i].enabled = value;
    }
    for (int i = row + 1; i < static_cast<int>(m_visibleIndices.size()); ++i) {
      auto &child = m_allEntries[m_visibleIndices[i]];
      if (child.isProvider) break;
      auto childIdx = index(i);
      emit dataChanged(childIdx, childIdx, {EnabledRole});
    }
  } else {
    manager->setItemEnabled(e.entrypointId, value);
    e.enabled = value;
    auto idx = index(row);
    emit dataChanged(idx, idx, {EnabledRole});
  }

  if (row == m_selectedRow) emit selectedChanged();
}

void ExtensionSettingsModel::setAlias(int row, const QString &alias) {
  if (row < 0 || row >= static_cast<int>(m_visibleIndices.size())) return;
  auto &e = m_allEntries[m_visibleIndices[row]];
  if (e.isProvider) return;
  auto *manager = ServiceRegistry::instance()->rootItemManager();
  manager->setAlias(e.entrypointId, alias.toStdString());
  e.alias = alias;
  auto idx = index(row);
  emit dataChanged(idx, idx, {AliasRole});
  if (row == m_selectedRow) emit selectedChanged();
}

void ExtensionSettingsModel::selectByEntrypointId(const QString &id) {
  for (int i = 0; i < static_cast<int>(m_visibleIndices.size()); ++i) {
    auto &e = m_allEntries[m_visibleIndices[i]];
    if (!e.isProvider && QString::fromStdString(e.entrypointId) == id) {
      for (int j = i - 1; j >= 0; --j) {
        auto &parent = m_allEntries[m_visibleIndices[j]];
        if (parent.isProvider) {
          if (!parent.expanded) toggleExpanded(j);
          break;
        }
      }
      for (int k = 0; k < static_cast<int>(m_visibleIndices.size()); ++k) {
        auto &e2 = m_allEntries[m_visibleIndices[k]];
        if (!e2.isProvider && QString::fromStdString(e2.entrypointId) == id) {
          select(k);
          return;
        }
      }
      return;
    }
  }
  for (int i = 0; i < static_cast<int>(m_allEntries.size()); ++i) {
    auto &e = m_allEntries[i];
    if (!e.isProvider && QString::fromStdString(e.entrypointId) == id) {
      for (int j = i - 1; j >= 0; --j) {
        if (m_allEntries[j].isProvider) {
          for (int k = 0; k < static_cast<int>(m_visibleIndices.size()); ++k) {
            if (m_visibleIndices[k] == j && !m_allEntries[j].expanded) {
              toggleExpanded(k);
              break;
            }
          }
          break;
        }
      }
      for (int k = 0; k < static_cast<int>(m_visibleIndices.size()); ++k) {
        if (m_visibleIndices[k] == i) {
          select(k);
          return;
        }
      }
      return;
    }
  }
}

void ExtensionSettingsModel::toggleExpanded(int row) {
  if (row < 0 || row >= static_cast<int>(m_visibleIndices.size())) return;
  int allIdx = m_visibleIndices[row];
  auto &e = m_allEntries[allIdx];
  if (!e.isProvider) return;

  e.expanded = !e.expanded;

  if (e.expanded) {
    m_expandedProviders.insert(e.providerId);
  } else {
    m_expandedProviders.erase(e.providerId);
  }

  int childrenVisible = 0;
  for (int i = row + 1; i < static_cast<int>(m_visibleIndices.size()); ++i) {
    if (m_allEntries[m_visibleIndices[i]].isProvider) break;
    ++childrenVisible;
  }

  if (e.expanded) {
    std::vector<int> toInsert;
    for (int i = allIdx + 1; i < static_cast<int>(m_allEntries.size()) && !m_allEntries[i].isProvider; ++i) {
      toInsert.push_back(i);
    }
    if (!toInsert.empty()) {
      beginInsertRows({}, row + 1, row + static_cast<int>(toInsert.size()));
      m_visibleIndices.insert(m_visibleIndices.begin() + row + 1, toInsert.begin(), toInsert.end());
      endInsertRows();
    }
  } else {
    if (childrenVisible > 0) {
      if (m_selectedRow > row && m_selectedRow <= row + childrenVisible) {
        m_selectedRow = row;
        emit selectedChanged();
      } else if (m_selectedRow > row + childrenVisible) {
        m_selectedRow -= childrenVisible;
      }
      beginRemoveRows({}, row + 1, row + childrenVisible);
      m_visibleIndices.erase(m_visibleIndices.begin() + row + 1,
                             m_visibleIndices.begin() + row + 1 + childrenVisible);
      endRemoveRows();
    }
  }

  auto idx = index(row);
  emit dataChanged(idx, idx, {ExpandedRole});
}

void ExtensionSettingsModel::moveUp() {
  if (m_selectedRow > 0) select(m_selectedRow - 1);
}

void ExtensionSettingsModel::moveDown() {
  if (m_selectedRow < rowCount() - 1) select(m_selectedRow + 1);
}

void ExtensionSettingsModel::activate() {
  if (hasSelection() && m_allEntries[m_visibleIndices[m_selectedRow]].isProvider)
    toggleExpanded(m_selectedRow);
}

void ExtensionSettingsModel::rebuild(const QString &filter) {
  beginResetModel();
  m_allEntries.clear();
  m_visibleIndices.clear();

  auto *manager = ServiceRegistry::instance()->rootItemManager();
  RootItemPrefixSearchOptions opts;
  opts.includeDisabled = true;

  std::vector<std::pair<std::string, std::vector<std::shared_ptr<RootItem>>>> providerMap;

  for (const auto &scored : manager->search(filter, opts)) {
    auto &itemPtr = scored.item.get();
    auto id = itemPtr->uniqueId();
    auto pred = [&](auto &&pair) { return pair.first == id.provider; };
    if (auto it = std::ranges::find_if(providerMap, pred); it != providerMap.end()) {
      it->second.emplace_back(itemPtr);
    } else {
      providerMap.push_back({id.provider, {itemPtr}});
    }
  }

  bool isFiltering = !filter.isEmpty();

  for (const auto &[providerId, items] : providerMap) {
    auto *provider = manager->provider(providerId);
    if (!provider || provider->isTransient()) continue;

    Entry pe;
    pe.name = provider->displayName();
    pe.type = provider->typeAsString();
    pe.iconSource = qml::imageSourceFor(provider->icon());
    pe.isProvider = true;
    pe.indent = 0;
    pe.enabled = true;
    pe.providerId = provider->uniqueId();
    pe.childCount = static_cast<int>(items.size());
    pe.expanded = isFiltering || m_expandedProviders.count(pe.providerId) > 0;

    if (auto *ext = dynamic_cast<ExtensionRootProvider *>(provider))
      pe.description = ext->repository()->description();

    m_allEntries.push_back(std::move(pe));

    for (const auto &item : items) {
      auto metadata = manager->itemMetadata(item->uniqueId());
      Entry ie;
      ie.name = item->displayName();
      ie.type = item->typeDisplayName();
      ie.iconSource = qml::imageSourceFor(item->iconUrl());
      ie.isProvider = false;
      ie.indent = 1;
      ie.enabled = metadata.enabled;
      ie.alias = QString::fromStdString(metadata.alias.value_or(""));
      ie.entrypointId = item->uniqueId();
      ie.providerId = provider->uniqueId();
      ie.description = item->settingsDescription();
      for (const auto &[key, value] : item->settingsMetadata())
        ie.metadata.append(QVariantMap{{QStringLiteral("key"), key}, {QStringLiteral("value"), value}});
      m_allEntries.push_back(std::move(ie));
    }
  }

  rebuildVisible();

  endResetModel();

  int newRow = !filter.isEmpty() ? 0 : m_selectedRow;
  if (newRow >= static_cast<int>(m_visibleIndices.size())) newRow = m_visibleIndices.empty() ? -1 : 0;
  m_selectedRow = -1;
  if (newRow == -1)
    emit selectedChanged();
  else
    select(newRow);
}

void ExtensionSettingsModel::rebuildVisible() {
  m_visibleIndices.clear();
  bool skipChildren = false;
  for (int i = 0; i < static_cast<int>(m_allEntries.size()); ++i) {
    auto &e = m_allEntries[i];
    if (e.isProvider) {
      m_visibleIndices.push_back(i);
      skipChildren = !e.expanded;
    } else {
      if (!skipChildren) m_visibleIndices.push_back(i);
    }
  }
}
