#include "qml-command-grid-model.hpp"
#include "navigation-controller.hpp"
#include <algorithm>

QmlCommandGridModel::QmlCommandGridModel(QObject *parent) : QAbstractListModel(parent) {}

void QmlCommandGridModel::initialize(ApplicationContext *ctx) { m_ctx = ctx; }

int QmlCommandGridModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_rows.size());
}

void QmlCommandGridModel::setSelectedIndex(int index) { (void)index; }

QVariant QmlCommandGridModel::data(const QModelIndex &index, int role) const {
  int row = index.row();
  if (row < 0 || row >= static_cast<int>(m_rows.size())) return {};
  const auto &r = m_rows[row];

  switch (role) {
  case IsSection:
    return r.kind == FlatRow::SectionHeader;
  case SectionNameRole:
    return r.sectionName;
  case RowSectionIdx:
    return r.sectionIdx;
  case RowStartItem:
    return r.startItem;
  case RowItemCount:
    return r.itemCount;
  case RowColumnsRole:
    return r.columns;
  case RowAspectRatioRole:
    return r.aspectRatio;
  default:
    return {};
  }
}

QHash<int, QByteArray> QmlCommandGridModel::roleNames() const {
  return {
      {IsSection, "isSection"},
      {SectionNameRole, "sectionName"},
      {RowSectionIdx, "rowSectionIdx"},
      {RowStartItem, "rowStartItem"},
      {RowItemCount, "rowItemCount"},
      {RowColumnsRole, "rowColumns"},
      {RowAspectRatioRole, "rowAspectRatio"},
  };
}

std::vector<QmlCommandGridModel::FlatRow>
QmlCommandGridModel::buildFlatList(const std::vector<SectionInfo> &sections) {
  std::vector<FlatRow> rows;

  for (int s = 0; s < static_cast<int>(sections.size()); ++s) {
    const auto &sec = sections[s];
    if (sec.count == 0) continue;

    int cols = sec.columns.value_or(m_columns);
    double ar = sec.aspectRatio.value_or(m_aspectRatio);

    if (!sec.name.isEmpty()) {
      rows.push_back({FlatRow::SectionHeader, s, sec.name, 0, 0, cols, ar});
    }

    for (int i = 0; i < sec.count; i += cols) {
      int count = std::min(cols, sec.count - i);
      rows.push_back({FlatRow::ItemRow, s, {}, i, count, cols, ar});
    }
  }

  return rows;
}

void QmlCommandGridModel::setSections(const std::vector<SectionInfo> &sections) {
  auto newFlat = buildFlatList(sections);
  int oldCount = static_cast<int>(m_rows.size());
  int newCount = static_cast<int>(newFlat.size());

  if (m_selectFirstOnReset) {
    beginResetModel();
    m_sections = sections;
    m_rows = std::move(newFlat);
    m_selectedIndex = -1;
    endResetModel();
    return;
  }

  if (newCount < oldCount) {
    beginRemoveRows({}, newCount, oldCount - 1);
    m_sections = sections;
    m_rows = std::move(newFlat);
    endRemoveRows();
  } else if (newCount > oldCount) {
    beginInsertRows({}, oldCount, newCount - 1);
    m_sections = sections;
    m_rows = std::move(newFlat);
    endInsertRows();
  } else {
    m_sections = sections;
    m_rows = std::move(newFlat);
  }

  int overlap = std::min(oldCount, newCount);
  if (overlap > 0) emit dataChanged(index(0), index(overlap - 1));

  if (m_selectedIndex >= newCount) m_selectedIndex = -1;
}

void QmlCommandGridModel::setColumns(int cols) {
  if (cols < 1) cols = 1;
  if (cols == m_columns) return;
  m_columns = cols;
  emit columnsChanged();
  rebuildRows();
  if (m_selSection >= 0 && m_selItem >= 0) {
    int s = m_selSection, i = m_selItem;
    m_selSection = -1;
    m_selItem = -1;
    select(s, i);
  }
}

void QmlCommandGridModel::setAspectRatio(double ratio) {
  if (ratio <= 0.0) ratio = 1.0;
  if (qFuzzyCompare(ratio, m_aspectRatio)) return;
  m_aspectRatio = ratio;
  emit aspectRatioChanged();
  rebuildRows();
}

int QmlCommandGridModel::sectionColumns(int sectionIdx) const {
  if (sectionIdx < 0 || sectionIdx >= static_cast<int>(m_sections.size())) return m_columns;
  return m_sections[sectionIdx].columns.value_or(m_columns);
}

void QmlCommandGridModel::rebuildRows() { setSections(m_sections); }

void QmlCommandGridModel::select(int section, int item) {
  if (section == m_selSection && item == m_selItem) return;
  m_selSection = section;
  m_selItem = item;
  emit selectionChanged();

  onItemSelected(section, item);

  auto panel = createActionPanel(section, item);
  if (panel) {
    panel->finalize();
    m_ctx->navigation->setActions(std::move(panel));
  }
}

void QmlCommandGridModel::selectFirst() {
  for (int s = 0; s < static_cast<int>(m_sections.size()); ++s) {
    if (m_sections[s].count > 0) {
      select(s, 0);
      return;
    }
  }
  m_selSection = -1;
  m_selItem = -1;
  emit selectionChanged();
  onSelectionCleared();
}

void QmlCommandGridModel::onSelectionCleared() {
  if (m_ctx) m_ctx->navigation->clearActions();
}

void QmlCommandGridModel::activateSelected() {
  if (!m_ctx) return;
  m_ctx->navigation->executePrimaryAction();
}

void QmlCommandGridModel::refreshActionPanel() {
  if (m_selSection >= 0 && m_selItem >= 0) {
    auto panel = createActionPanel(m_selSection, m_selItem);
    if (panel) {
      panel->finalize();
      m_ctx->navigation->setActions(std::move(panel));
    }
  }
}

int QmlCommandGridModel::totalItemCount() const {
  int total = 0;
  for (const auto &s : m_sections)
    total += s.count;
  return total;
}

int QmlCommandGridModel::toGlobal(int section, int item) const {
  int idx = 0;
  for (int s = 0; s < section; ++s)
    idx += m_sections[s].count;
  return idx + item;
}

void QmlCommandGridModel::fromGlobal(int globalIdx, int &section, int &item) const {
  int offset = 0;
  for (int s = 0; s < static_cast<int>(m_sections.size()); ++s) {
    if (globalIdx < offset + m_sections[s].count) {
      section = s;
      item = globalIdx - offset;
      return;
    }
    offset += m_sections[s].count;
  }
  section = -1;
  item = -1;
}

void QmlCommandGridModel::navigateRight() {
  int g = toGlobal(m_selSection, m_selItem) + 1;
  int total = totalItemCount();
  if (total == 0) return;
  if (g >= total) g = 0;
  int s, i;
  fromGlobal(g, s, i);
  select(s, i);
}

void QmlCommandGridModel::navigateLeft() {
  int g = toGlobal(m_selSection, m_selItem) - 1;
  int total = totalItemCount();
  if (total == 0) return;
  if (g < 0) g = total - 1;
  int s, i;
  fromGlobal(g, s, i);
  select(s, i);
}

void QmlCommandGridModel::navigateDown() {
  if (m_selSection < 0) return;

  int cols = sectionColumns(m_selSection);
  int col = m_selItem % cols;
  int nextRow = (m_selItem / cols) + 1;
  int maxRowInSection = (m_sections[m_selSection].count - 1) / cols;

  if (nextRow <= maxRowInSection) {
    int targetItem = nextRow * cols + col;
    if (targetItem >= m_sections[m_selSection].count) targetItem = m_sections[m_selSection].count - 1;
    select(m_selSection, targetItem);
    return;
  }

  for (int s = m_selSection + 1; s < static_cast<int>(m_sections.size()); ++s) {
    if (m_sections[s].count > 0) {
      int targetCols = sectionColumns(s);
      int targetItem = std::min(col, std::min(targetCols, m_sections[s].count) - 1);
      select(s, targetItem);
      return;
    }
  }

  for (int s = 0; s < static_cast<int>(m_sections.size()); ++s) {
    if (m_sections[s].count > 0) {
      int targetCols = sectionColumns(s);
      int targetItem = std::min(col, std::min(targetCols, m_sections[s].count) - 1);
      select(s, targetItem);
      return;
    }
  }
}

void QmlCommandGridModel::navigateUp() {
  if (m_selSection < 0) return;

  int cols = sectionColumns(m_selSection);
  int col = m_selItem % cols;
  int currentRow = m_selItem / cols;

  if (currentRow > 0) {
    select(m_selSection, (currentRow - 1) * cols + col);
    return;
  }

  for (int s = m_selSection - 1; s >= 0; --s) {
    if (m_sections[s].count > 0) {
      int targetCols = sectionColumns(s);
      int lastRow = (m_sections[s].count - 1) / targetCols;
      int targetCol = std::min(col, targetCols - 1);
      int targetItem = lastRow * targetCols + targetCol;
      if (targetItem >= m_sections[s].count) targetItem = m_sections[s].count - 1;
      select(s, targetItem);
      return;
    }
  }

  for (int s = static_cast<int>(m_sections.size()) - 1; s >= 0; --s) {
    if (m_sections[s].count > 0) {
      int targetCols = sectionColumns(s);
      int lastRow = (m_sections[s].count - 1) / targetCols;
      int targetCol = std::min(col, targetCols - 1);
      int targetItem = lastRow * targetCols + targetCol;
      if (targetItem >= m_sections[s].count) targetItem = m_sections[s].count - 1;
      select(s, targetItem);
      return;
    }
  }
}

int QmlCommandGridModel::flatRowForSelection() const {
  if (m_selSection < 0 || m_selItem < 0) return -1;
  for (int r = 0; r < static_cast<int>(m_rows.size()); ++r) {
    const auto &row = m_rows[r];
    if (row.kind == FlatRow::ItemRow && row.sectionIdx == m_selSection && m_selItem >= row.startItem &&
        m_selItem < row.startItem + row.itemCount) {
      // When the selected cell is in the first item row of a section that has
      // a header, return the header row so positionViewAtIndex also keeps
      // the section title visible.
      if (row.startItem == 0 && r > 0 && m_rows[r - 1].kind == FlatRow::SectionHeader &&
          m_rows[r - 1].sectionIdx == m_selSection) {
        return r - 1;
      }
      return r;
    }
  }
  return -1;
}
