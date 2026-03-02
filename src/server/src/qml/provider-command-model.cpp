#include "provider-command-model.hpp"
#include <utility>

ProviderCommandModel::ProviderCommandModel(QObject *parent) : QAbstractListModel(parent) {}

int ProviderCommandModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_visibleIndices.size());
}

QVariant ProviderCommandModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_visibleIndices.size()))
    return {};
  const auto &cmd = m_allCommands[m_visibleIndices[index.row()]];
  switch (role) {
  case NameRole:
    return cmd.name;
  case TypeRole:
    return cmd.type;
  case IconSourceRole:
    return cmd.iconSource;
  case EnabledRole:
    return cmd.enabled;
  case AliasRole:
    return cmd.alias;
  case EntrypointIdRole:
    return cmd.entrypointId;
  case DescriptionRole:
    return cmd.description;
  case HasPreferencesRole:
    return cmd.hasPreferences;
  default:
    return {};
  }
}

QHash<int, QByteArray> ProviderCommandModel::roleNames() const {
  return {{NameRole, "name"},
          {TypeRole, "type"},
          {IconSourceRole, "iconSource"},
          {EnabledRole, "enabled"},
          {AliasRole, "alias"},
          {EntrypointIdRole, "entrypointId"},
          {DescriptionRole, "description"},
          {HasPreferencesRole, "hasPreferences"}};
}

void ProviderCommandModel::load(std::vector<Command> commands) {
  int const oldCount = rowCount();
  int const oldTotal = totalCount();
  m_filter.clear();
  beginResetModel();
  m_allCommands = std::move(commands);
  rebuildVisible();
  endResetModel();
  if (rowCount() != oldCount) emit countChanged();
  if (totalCount() != oldTotal) emit totalCountChanged();
}

void ProviderCommandModel::clear() {
  if (m_allCommands.empty()) return;
  beginResetModel();
  m_allCommands.clear();
  m_visibleIndices.clear();
  endResetModel();
  emit countChanged();
  emit totalCountChanged();
}

void ProviderCommandModel::setFilter(const QString &text) {
  if (m_filter == text) return;
  m_filter = text;
  int const oldCount = rowCount();
  beginResetModel();
  rebuildVisible();
  endResetModel();
  if (rowCount() != oldCount) emit countChanged();
}

void ProviderCommandModel::rebuildVisible() {
  auto query = m_filter.toStdString();
  fuzzy::fuzzyFilter<Command>(m_allCommands, query, m_scored);
  m_visibleIndices.clear();
  m_visibleIndices.reserve(m_scored.size());
  for (const auto &s : m_scored) {
    m_visibleIndices.push_back(s.data);
  }
}

bool ProviderCommandModel::setEnabled(const QString &entrypointId, bool value) {
  for (int i = 0; std::cmp_less(i, m_allCommands.size()); ++i) {
    if (m_allCommands[i].entrypointId != entrypointId) continue;
    m_allCommands[i].enabled = value;
    int const row = visibleRowFor(i);
    if (row >= 0) {
      auto idx = index(row);
      emit dataChanged(idx, idx, {EnabledRole});
    }
    return true;
  }
  return false;
}

void ProviderCommandModel::setAllEnabled(bool value) {
  for (auto &cmd : m_allCommands) {
    cmd.enabled = value;
  }
  if (!m_visibleIndices.empty()) {
    emit dataChanged(index(0), index(static_cast<int>(m_visibleIndices.size()) - 1), {EnabledRole});
  }
}

bool ProviderCommandModel::setAlias(const QString &entrypointId, const QString &alias) {
  for (int i = 0; std::cmp_less(i, m_allCommands.size()); ++i) {
    if (m_allCommands[i].entrypointId != entrypointId) continue;
    m_allCommands[i].alias = alias;
    int const row = visibleRowFor(i);
    if (row >= 0) {
      auto idx = index(row);
      emit dataChanged(idx, idx, {AliasRole});
    }
    return true;
  }
  return false;
}

int ProviderCommandModel::findByEntrypointId(const QString &id) const {
  for (int i = 0; std::cmp_less(i, m_visibleIndices.size()); ++i) {
    if (m_allCommands[m_visibleIndices[i]].entrypointId == id) return i;
  }
  return -1;
}

int ProviderCommandModel::visibleRowFor(int allIdx) const {
  for (int i = 0; std::cmp_less(i, m_visibleIndices.size()); ++i) {
    if (m_visibleIndices[i] == allIdx) return i;
  }
  return -1;
}
