#include "provider-command-model.hpp"
#include <utility>

ProviderCommandModel::ProviderCommandModel(QObject *parent) : QAbstractListModel(parent) {}

int ProviderCommandModel::rowCount(const QModelIndex &) const { return static_cast<int>(m_commands.size()); }

QVariant ProviderCommandModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_commands.size())) return {};
  const auto &cmd = m_commands[index.row()];
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
  int oldCount = rowCount();
  if (!m_commands.empty()) {
    beginRemoveRows({}, 0, static_cast<int>(m_commands.size()) - 1);
    m_commands.clear();
    endRemoveRows();
  }
  if (!commands.empty()) {
    beginInsertRows({}, 0, static_cast<int>(commands.size()) - 1);
    m_commands = std::move(commands);
    endInsertRows();
  }
  if (rowCount() != oldCount)
    emit countChanged();
}

void ProviderCommandModel::clear() {
  if (m_commands.empty()) return;
  beginRemoveRows({}, 0, static_cast<int>(m_commands.size()) - 1);
  m_commands.clear();
  endRemoveRows();
  emit countChanged();
}

bool ProviderCommandModel::setEnabled(const QString &entrypointId, bool value) {
  int row = findByEntrypointId(entrypointId);
  if (row < 0) return false;
  m_commands[row].enabled = value;
  auto idx = index(row);
  emit dataChanged(idx, idx, {EnabledRole});
  return true;
}

bool ProviderCommandModel::setAlias(const QString &entrypointId, const QString &alias) {
  int row = findByEntrypointId(entrypointId);
  if (row < 0) return false;
  m_commands[row].alias = alias;
  auto idx = index(row);
  emit dataChanged(idx, idx, {AliasRole});
  return true;
}

int ProviderCommandModel::findByEntrypointId(const QString &id) const {
  for (int i = 0; i < static_cast<int>(m_commands.size()); ++i) {
    if (m_commands[i].entrypointId == id) return i;
  }
  return -1;
}
