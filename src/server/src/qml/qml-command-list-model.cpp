#include "qml-command-list-model.hpp"
#include "navigation-controller.hpp"

void QmlCommandListModel::initialize(ApplicationContext *ctx) { m_ctx = ctx; }

int QmlCommandListModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_flat.size());
}

QVariant QmlCommandListModel::data(const QModelIndex &index, int role) const {
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
      return QString();
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

QHash<int, QByteArray> QmlCommandListModel::roleNames() const {
  return {
      {IsSection, "isSection"},     {IsSelectable, "isSelectable"},
      {SectionName, "sectionName"}, {Title, "title"},
      {Subtitle, "subtitle"},       {IconSource, "iconSource"},
      {Accessory, "itemAccessory"},
  };
}

void QmlCommandListModel::setSelectedIndex(int index) {
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

  if (!sameItem) {
    onItemSelected(flat.sectionIdx, flat.itemIdx);
  }

  auto panel = createActionPanel(flat.sectionIdx, flat.itemIdx);
  if (panel) {
    panel->finalize();
    if (m_ctx) m_ctx->navigation->setActions(std::move(panel));
  }
}

void QmlCommandListModel::onSelectionCleared() {
  if (m_ctx) m_ctx->navigation->clearActions();
}

void QmlCommandListModel::refreshActionPanel() {
  setSelectedIndex(m_selectedIndex);
}

void QmlCommandListModel::activateSelected() {
  if (!m_ctx) return;
  m_ctx->navigation->executePrimaryAction();
}

int QmlCommandListModel::nextSelectableIndex(int from, int direction) const {
  int idx = from + direction;
  int count = static_cast<int>(m_flat.size());

  while (idx >= 0 && idx < count) {
    if (m_flat[idx].kind != FlatItem::SectionHeader) return idx;
    idx += direction;
  }

  return from;
}

void QmlCommandListModel::setSections(const std::vector<SectionInfo> &sections) {
  beginResetModel();
  m_sectionInfos = sections;
  m_selectedIndex = -1;
  rebuildFlatList();
  endResetModel();
}

bool QmlCommandListModel::dataItemAt(int row, int &section, int &item) const {
  if (row < 0 || row >= static_cast<int>(m_flat.size())) return false;
  const auto &flat = m_flat[row];
  if (flat.kind == FlatItem::SectionHeader) return false;
  section = flat.sectionIdx;
  item = flat.itemIdx;
  return true;
}

void QmlCommandListModel::rebuildFlatList() {
  m_flat.clear();

  for (int s = 0; s < static_cast<int>(m_sectionInfos.size()); ++s) {
    const auto &sec = m_sectionInfos[s];
    if (sec.count == 0) continue;

    m_flat.push_back({.kind = FlatItem::SectionHeader, .sectionIdx = s, .itemIdx = -1});

    for (int i = 0; i < sec.count; ++i) {
      m_flat.push_back({.kind = FlatItem::DataItem, .sectionIdx = s, .itemIdx = i});
    }
  }
}
