#include "command-list-model.hpp"

int CommandListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant CommandListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size())) return {};

  const auto &flat = m_flat[index.row()];

  if (flat.kind == FlatItem::SectionHeader) {
    switch (role) {
    case IsSection:
      return true;
    case IsSelectable:
      return false;
    case SectionName:
      return m_sectionInfos[flat.sectionIdx].name;
    case Title:
      return QString();
    case Subtitle:
      return QString();
    case IconSource:
      return QString();
    case Accessory:
      return {};
    default:
      return {};
    }
  }

  switch (role) {
  case IsSection:
    return false;
  case IsSelectable:
    return true;
  case SectionName:
    return QString();
  case Title:
    return itemTitle(flat.sectionIdx, flat.itemIdx);
  case Subtitle:
    return itemSubtitle(flat.sectionIdx, flat.itemIdx);
  case IconSource:
    return itemIconSource(flat.sectionIdx, flat.itemIdx);
  case Accessory:
    return itemAccessory(flat.sectionIdx, flat.itemIdx);
  default:
    return {};
  }
}

QHash<int, QByteArray> CommandListModel::roleNames() const {
  return {
      {IsSection, "isSection"},     {IsSelectable, "isSelectable"},
      {SectionName, "sectionName"}, {Title, "title"},
      {Subtitle, "subtitle"},       {IconSource, "iconSource"},
      {Accessory, "itemAccessory"},
  };
}

void CommandListModel::setSelectedIndex(int index) {
  m_selectedIndex = index;

  if (index < 0 || index >= static_cast<int>(m_flat.size())) {
    m_lastSelectedItemId.clear();
    onSelectionCleared();
    return;
  }

  const auto &flat = m_flat[index];
  if (flat.kind == FlatItem::SectionHeader) {
    m_lastSelectedItemId.clear();
    onSelectionCleared();
    return;
  }

  auto id = itemId(flat.sectionIdx, flat.itemIdx);
  bool sameItem = (id == m_lastSelectedItemId);
  m_lastSelectedItemId = id;

  if (!sameItem) { onItemSelected(flat.sectionIdx, flat.itemIdx); }

  auto panel = createActionPanel(flat.sectionIdx, flat.itemIdx);
  if (panel) { m_scope.setActions(std::move(panel)); }
}

void CommandListModel::onSelectionCleared() { m_scope.clearActions(); }

void CommandListModel::refreshActionPanel() { setSelectedIndex(m_selectedIndex); }

void CommandListModel::activateSelected() { m_scope.executePrimaryAction(); }

int CommandListModel::nextSelectableIndex(int from, int direction) const {
  int idx = from + direction;
  int count = static_cast<int>(m_flat.size());

  while (idx >= 0 && idx < count) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
  }

  return from;
}

int CommandListModel::scrollTargetIndex(int index, int direction) const {
  if (direction < 0 && index > 0 && index < static_cast<int>(m_flat.size())) {
    if (m_flat[index - 1].kind == FlatItem::SectionHeader) return index - 1;
  }
  return index;
}

void CommandListModel::setSections(const std::vector<SectionInfo> &sections) {
  if (m_awaitingData) {
    m_awaitingData = false;
    emit awaitingDataChanged();
  }

  auto newFlat = buildFlatList(sections);
  int oldCount = static_cast<int>(m_flat.size());
  int newCount = static_cast<int>(newFlat.size());

  if (m_selectFirstOnReset) {
    beginResetModel();
    m_sectionInfos = sections;
    m_flat = std::move(newFlat);
    m_selectedIndex = -1;
    m_lastSelectedItemId.clear();
    endResetModel();
    return;
  }

  // Incremental update — preserves ListView scroll position and selection
  if (newCount < oldCount) {
    beginRemoveRows({}, newCount, oldCount - 1);
    m_sectionInfos = sections;
    m_flat = std::move(newFlat);
    endRemoveRows();
  } else if (newCount > oldCount) {
    beginInsertRows({}, oldCount, newCount - 1);
    m_sectionInfos = sections;
    m_flat = std::move(newFlat);
    endInsertRows();
  } else {
    m_sectionInfos = sections;
    m_flat = std::move(newFlat);
  }

  int overlap = std::min(oldCount, newCount);
  if (overlap > 0) emit dataChanged(index(0), index(overlap - 1));

  if (m_selectedIndex >= newCount) m_selectedIndex = -1;
}

bool CommandListModel::dataItemAt(int row, int &section, int &item) const {
  if (row < 0 || row >= static_cast<int>(m_flat.size())) return false;
  const auto &flat = m_flat[row];
  if (flat.kind == FlatItem::SectionHeader) return false;
  section = flat.sectionIdx;
  item = flat.itemIdx;
  return true;
}

std::vector<CommandListModel::FlatItem>
CommandListModel::buildFlatList(const std::vector<SectionInfo> &sections) {
  std::vector<FlatItem> flat;

  for (int s = 0; s < static_cast<int>(sections.size()); ++s) {
    const auto &sec = sections[s];
    if (sec.count == 0) continue;

    if (!sec.name.isEmpty())
      flat.push_back({.kind = FlatItem::SectionHeader, .sectionIdx = s, .itemIdx = -1});

    for (int i = 0; i < sec.count; ++i) {
      flat.push_back({.kind = FlatItem::DataItem, .sectionIdx = s, .itemIdx = i});
    }
  }

  return flat;
}
