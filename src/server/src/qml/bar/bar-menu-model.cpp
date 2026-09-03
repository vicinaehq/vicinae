#include "bar/bar-menu-model.hpp"
#include <QBuffer>
#include <ranges>

void BarMenuModel::setEntries(std::vector<TrayMenuItem> entries) {
  beginResetModel();
  m_root = std::move(entries);
  m_path.clear();
  rebuildLevel();
  endResetModel();
  emit levelChanged();
}

void BarMenuModel::clear() { setEntries({}); }

const std::vector<TrayMenuItem> &BarMenuModel::currentLevel() const {
  const std::vector<TrayMenuItem> *level = &m_root;
  for (int idx : m_path) {
    level = &(*level)[idx].children;
  }
  return *level;
}

void BarMenuModel::rebuildLevel() {
  m_level.clear();
  const auto &level = currentLevel();
  m_level.reserve(level.size());
  for (const auto &entry : level) {
    if (entry.visible) m_level.emplace_back(&entry);
  }
}

QString BarMenuModel::title() const {
  if (m_path.empty()) return {};
  const std::vector<TrayMenuItem> *level = &m_root;
  const TrayMenuItem *entry = nullptr;
  for (int idx : m_path) {
    entry = &(*level)[idx];
    level = &entry->children;
  }
  return entry ? entry->plainLabel() : QString();
}

void BarMenuModel::enter(int row) {
  if (row < 0 || std::cmp_greater_equal(row, m_level.size())) return;
  const auto *entry = m_level[row];
  if (!entry->submenu) return;
  const auto &level = currentLevel();
  const auto pos = entry - level.data();
  beginResetModel();
  m_path.emplace_back(static_cast<int>(pos));
  rebuildLevel();
  endResetModel();
  emit levelChanged();
}

void BarMenuModel::back() {
  if (m_path.empty()) return;
  beginResetModel();
  m_path.pop_back();
  rebuildLevel();
  endResetModel();
  emit levelChanged();
}

int BarMenuModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_level.size());
}

QVariant BarMenuModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || std::cmp_greater_equal(index.row(), m_level.size())) return {};
  const auto &entry = *m_level[index.row()];

  switch (role) {
  case Id:
    return entry.id;
  case Label:
    return entry.plainLabel();
  case Enabled:
    return entry.enabled;
  case Separator:
    return entry.separator;
  case Submenu:
    return entry.submenu;
  case ToggleType:
    return static_cast<int>(entry.toggleType);
  case Checked:
    return entry.toggleState == 1;
  case HasIcon:
    return !entry.iconData.isNull() || !entry.iconName.isEmpty();
  case Icon: {
    if (!entry.iconData.isNull()) {
      QByteArray bytes;
      QBuffer buf(&bytes);
      buf.open(QIODevice::WriteOnly);
      entry.iconData.save(&buf, "PNG");
      return QVariant::fromValue(ImageUrl(ImageURL::rawData(bytes, "image/png")));
    }
    if (!entry.iconName.isEmpty()) return QVariant::fromValue(ImageUrl(ImageURL::system(entry.iconName)));
    return QVariant::fromValue(ImageUrl());
  }
  default:
    return {};
  }
}

QHash<int, QByteArray> BarMenuModel::roleNames() const {
  return {{Id, "menuId"},           {Label, "label"},     {Enabled, "menuEnabled"},
          {Separator, "separator"}, {Submenu, "submenu"}, {ToggleType, "toggleType"},
          {Checked, "checked"},     {Icon, "icon"},       {HasIcon, "hasIcon"}};
}
