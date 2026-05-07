#include "section-list-model.hpp"

#include <utility>

SectionListModel::SectionListModel(QObject *parent) : QAbstractListModel(parent) {
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    if (rowCount() > 0) emit dataChanged(index(0), index(rowCount() - 1), {IconSource});
  });
}

void SectionListModel::addSource(SectionSource *source) {
  m_sources.push_back(source);
  source->setScope(m_scope);
  source->setOnChanged([this](bool preserveSelection) {
    if (preserveSelection && !m_awaitingData) {
      auto saved = m_selectFirstOnReset;
      m_selectFirstOnReset = false;
      rebuild();
      m_selectFirstOnReset = saved;
      refreshActionPanel();
    } else {
      rebuild();
    }
  });
  rebuildCustomRoleDefaults();
}

void SectionListModel::clearSources() {
  for (auto *source : m_sources)
    source->setOnChanged(nullptr);
  m_sources.clear();
  m_customRoleDefaults.clear();
}

void SectionListModel::setSelectFirstOnReset(bool value) {
  if (m_selectFirstOnReset != value) {
    m_selectFirstOnReset = value;
    emit selectFirstOnResetChanged();
  }
}

void SectionListModel::rebuild() { rebuildFlatList(); }

void SectionListModel::setFilter(const QString &text) {
  auto query = text.toStdString();
  for (auto *source : m_sources) {
    source->setFilter(query);
  }
  rebuild();
}

int SectionListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant SectionListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_flat.size())) return {};

  const auto &flat = m_flat[index.row()];

  if (flat.kind == FlatItem::SectionHeader) {
    switch (role) {
    case IsSection:
      return true;
    case IsSelectable:
      return false;
    case SectionName: {
      auto *source = m_sources[flat.sourceIdx];
      auto name = source->sectionName();
      name.replace(QStringLiteral("{count}"), QString::number(source->count()));
      return name;
    }
    case Title:
      return QString();
    case Subtitle:
      return QString();
    case IconSource:
      return QString();
    case Accessory:
      return {};
    default: {
      auto it = m_customRoleDefaults.find(role);
      return it != m_customRoleDefaults.end() ? it.value() : QVariant{};
    }
    }
  }

  auto *source = m_sources[flat.sourceIdx];

  switch (role) {
  case IsSection:
    return false;
  case IsSelectable:
    return true;
  case SectionName:
    return QString();
  case Title:
    return source->itemTitle(flat.itemIdx);
  case Subtitle:
    return source->itemSubtitle(flat.itemIdx);
  case IconSource:
    return source->itemIconSource(flat.itemIdx);
  case Accessory:
    return source->itemAccessories(flat.itemIdx);
  default: {
    auto v = source->customData(flat.itemIdx, role);
    if (v.isValid()) return v;
    auto it = m_customRoleDefaults.find(role);
    return it != m_customRoleDefaults.end() ? it.value() : QVariant{};
  }
  }
}

QHash<int, QByteArray> SectionListModel::roleNames() const {
  QHash<int, QByteArray> roles = {
      {IsSection, "isSection"},     {IsSelectable, "isSelectable"},
      {SectionName, "sectionName"}, {Title, "title"},
      {Subtitle, "subtitle"},       {IconSource, "iconSource"},
      {Accessory, "itemAccessory"},
  };
  for (auto *source : m_sources)
    roles.insert(source->customRoleNames());
  return roles;
}

void SectionListModel::setSelectedIndex(int index) {
  bool const changed = (m_selectedIndex != index);
  m_selectedIndex = index;
  if (changed) emit selectedIndexChanged();

  if (index < 0 || std::cmp_greater_equal(index, m_flat.size())) {
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

  auto *source = m_sources[flat.sourceIdx];
  auto id = source->itemId(flat.itemIdx);
  bool const sameItem = (id == m_lastSelectedItemId);
  m_lastSelectedItemId = id;

  if (!sameItem) { source->onSelected(flat.itemIdx); }

  auto panel = source->actionPanel(flat.itemIdx);
  if (panel) {
    m_scope.setActions(std::move(panel));
  } else {
    m_scope.clearActions();
  }
}

void SectionListModel::refreshActionPanel() { setSelectedIndex(m_selectedIndex); }

void SectionListModel::activateSelected() { m_scope.executePrimaryAction(); }

int SectionListModel::nextSelectableIndex(int from, int direction) const {
  int const count = static_cast<int>(m_flat.size());
  if (count == 0) return from;

  int idx = from + direction;
  if (idx < 0)
    idx = count - 1;
  else if (idx >= count)
    idx = 0;

  while (idx != from) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
    if (idx < 0)
      idx = count - 1;
    else if (idx >= count)
      idx = 0;
  }

  return from;
}

int SectionListModel::nextSectionIndex(int from, int direction) const {
  int const count = static_cast<int>(m_flat.size());
  if (count == 0) return from;

  int currentSource = -1;
  if (from >= 0 && from < count) { currentSource = m_flat[from].sourceIdx; }

  auto firstDataItemFrom = [&](int idx) -> int {
    while (idx < count && m_flat[idx].kind != FlatItem::DataItem)
      ++idx;
    return (idx < count) ? idx : from;
  };

  if (direction > 0) {
    for (int idx = from + 1; idx < count; ++idx) {
      if (m_flat[idx].kind == FlatItem::DataItem && m_flat[idx].sourceIdx != currentSource) { return idx; }
      if (m_flat[idx].kind == FlatItem::SectionHeader && m_flat[idx].sourceIdx != currentSource) {
        return firstDataItemFrom(idx + 1);
      }
    }
    return nextSelectableIndex(-1, 1);
  }

  int currentStart = from;
  for (int idx = from - 1; idx >= 0; --idx) {
    if (m_flat[idx].kind == FlatItem::DataItem && m_flat[idx].sourceIdx == currentSource) {
      currentStart = idx;
    } else if (m_flat[idx].sourceIdx != currentSource || m_flat[idx].kind == FlatItem::SectionHeader) {
      break;
    }
  }

  if (currentStart < from) { return currentStart; }

  for (int idx = currentStart - 1; idx >= 0; --idx) {
    if (m_flat[idx].kind == FlatItem::SectionHeader || m_flat[idx].sourceIdx != currentSource) {
      int prevSource = -1;
      for (int j = idx; j >= 0; --j) {
        if (m_flat[j].kind == FlatItem::DataItem) {
          prevSource = m_flat[j].sourceIdx;
          break;
        }
      }
      if (prevSource < 0) break;
      for (int j = 0; j < count; ++j) {
        if (m_flat[j].kind == FlatItem::DataItem && m_flat[j].sourceIdx == prevSource) return j;
      }
      break;
    }
  }

  int lastSource = -1;
  for (int i = count - 1; i >= 0; --i) {
    if (m_flat[i].kind == FlatItem::DataItem) {
      lastSource = m_flat[i].sourceIdx;
      break;
    }
  }
  if (lastSource >= 0) {
    for (int i = 0; i < count; ++i) {
      if (m_flat[i].kind == FlatItem::DataItem && m_flat[i].sourceIdx == lastSource) return i;
    }
  }
  return from;
}

int SectionListModel::scrollTargetIndex(int index, int direction) const {
  if (direction < 0 && index > 0 && std::cmp_less(index, m_flat.size())) {
    if (m_flat[index - 1].kind == FlatItem::SectionHeader) return index - 1;
  }
  return index;
}

void SectionListModel::onSelectionCleared() {
  for (auto *source : m_sources)
    source->onSelectionCleared();
  m_scope.clearActions();
}

void SectionListModel::beforePop() {}

bool SectionListModel::dataItemAt(int row, int &sourceIdx, int &itemIdx) const {
  if (row < 0 || std::cmp_greater_equal(row, m_flat.size())) return false;
  const auto &flat = m_flat[row];
  if (flat.kind != FlatItem::DataItem) return false;
  sourceIdx = flat.sourceIdx;
  itemIdx = flat.itemIdx;
  return true;
}

void SectionListModel::rebuildCustomRoleDefaults() {
  m_customRoleDefaults.clear();
  for (auto *source : m_sources)
    m_customRoleDefaults.insert(source->customRoleDefaults());
}

void SectionListModel::rebuildFlatList() {
  if (m_awaitingData) {
    m_awaitingData = false;
    emit awaitingDataChanged();
  }

  std::vector<FlatItem> newFlat;

  for (int s = 0; std::cmp_less(s, m_sources.size()); ++s) {
    auto *source = m_sources[s];
    int const itemCount = source->count();
    if (itemCount == 0) continue;

    auto name = source->sectionName();
    if (!name.isEmpty()) {
      newFlat.push_back({.kind = FlatItem::SectionHeader, .sourceIdx = s, .itemIdx = -1});
    }

    for (int i = 0; i < itemCount; ++i) {
      newFlat.push_back({.kind = FlatItem::DataItem, .sourceIdx = s, .itemIdx = i});
    }
  }

  int const oldCount = static_cast<int>(m_flat.size());
  int const newCount = static_cast<int>(newFlat.size());

  if (m_selectFirstOnReset) {
    beginResetModel();
    m_flat = std::move(newFlat);
    m_selectedIndex = -1;
    m_lastSelectedItemId.clear();
    endResetModel();
    return;
  }

  if (newCount < oldCount) {
    beginRemoveRows({}, newCount, oldCount - 1);
    m_flat = std::move(newFlat);
    endRemoveRows();
  } else if (newCount > oldCount) {
    beginInsertRows({}, oldCount, newCount - 1);
    m_flat = std::move(newFlat);
    endInsertRows();
  } else {
    m_flat = std::move(newFlat);
  }

  int const overlap = std::min(oldCount, newCount);
  if (overlap > 0) emit dataChanged(index(0), index(overlap - 1));

  int const prevSelected = m_selectedIndex;

  if (newCount == 0) {
    m_selectedIndex = -1;
  } else if (m_selectedIndex >= newCount) {
    m_selectedIndex = nextSelectableIndex(newCount, -1);
  } else if (m_selectedIndex >= 0 && m_flat[m_selectedIndex].kind == FlatItem::SectionHeader) {
    m_selectedIndex = nextSelectableIndex(m_selectedIndex, 1);
  }

  if (m_selectedIndex != prevSelected) emit selectedIndexChanged();
}
