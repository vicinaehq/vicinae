#include <QCache>
#include <QQmlEngine>
#include <QTextDocument>
#include <QTextOption>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include "document-table-model.hpp"
#include "document-text-images.hpp"
#include "text-document-edit.hpp"

namespace {
struct TextMetricsKey {
  QString html;
  QFont font;
  qreal width;
  qreal lineHeight;
  bool operator==(const TextMetricsKey &) const = default;
  friend std::size_t qHash(const TextMetricsKey &key, std::size_t seed = 0) {
    return qHashMulti(seed, key.html, key.font, key.width, key.lineHeight);
  }
};

qreal textHeight(const QString &html, const QFont &font, qreal width, qreal lineHeight,
                 DocumentImageCache *images) {
  // Retain intrinsic text measurements when virtualized table delegates are recreated.
  constexpr int CACHE_TEXT_UNITS = 1024 * 1024;
  static QCache<TextMetricsKey, qreal> cache(CACHE_TEXT_UNITS);
  const TextMetricsKey key{html, font, width, lineHeight};
  const bool hasImages = html.contains("<img", Qt::CaseInsensitive);
  if (!hasImages)
    if (const auto *height = cache.object(key)) return *height;
  QTextDocument document;
  if (hasImages) DocumentTextImages::measure(&document, images);
  document.setDocumentMargin(0);
  document.setDefaultFont(font);
  auto option = document.defaultTextOption();
  option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
  document.setDefaultTextOption(option);
  document.setHtml(html);
  if (lineHeight != 1) TextDocumentEdit::applyLineHeight(document, lineHeight);
  document.setTextWidth(width);
  const auto height = std::ceil(document.size().height());
  if (!hasImages) cache.insert(key, new qreal(height), html.size() + 64);
  return height;
}
} // namespace

int DocumentTableModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_layout.size());
}

QVariant DocumentTableModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) return {};
  const auto &row = m_layout[index.row()];
  switch (role) {
  case CellsRole:
    return row.cells;
  case HeaderRole:
    return row.header;
  case HeightRole:
    return row.height;
  case SelectionOffsetRole:
    return row.selectionOffset;
  default:
    return {};
  }
}

QHash<int, QByteArray> DocumentTableModel::roleNames() const {
  return {{CellsRole, "cells"},
          {HeaderRole, "header"},
          {HeightRole, "rowHeight"},
          {SelectionOffsetRole, "selectionOffset"}};
}

void DocumentTableModel::componentComplete() {
  m_complete = true;
  m_imageCache = DocumentImageCache::forEngine(qmlEngine(this));
  if (m_imageCache)
    connect(m_imageCache, &DocumentImageCache::sizesChanged, this, [this] {
      if (m_hasImages) rebuild();
    });
  rebuild();
}

void DocumentTableModel::setHeaders(const QVariantList &headers) {
  if (m_headers == headers) return;
  m_headers = headers;
  rebuild();
  emit headersChanged();
}

void DocumentTableModel::setRows(const QVariantList &rows) {
  if (m_rows == rows) return;
  m_rows = rows;
  rebuild();
  emit rowsChanged();
}

void DocumentTableModel::setFont(const QFont &font) {
  if (m_font == font) return;
  m_font = font;
  rebuild();
  emit fontChanged();
}

void DocumentTableModel::setCellWidth(qreal width) {
  if (m_cellWidth == width) return;
  m_cellWidth = width;
  rebuild();
  emit cellWidthChanged();
}

void DocumentTableModel::setCellPadding(qreal padding) {
  if (m_cellPadding == padding) return;
  m_cellPadding = padding;
  rebuild();
  emit cellPaddingChanged();
}

void DocumentTableModel::setLineHeight(qreal height) {
  if (m_lineHeight == height) return;
  m_lineHeight = height;
  rebuild();
  emit lineHeightChanged();
}

void DocumentTableModel::rebuild() {
  if (!m_complete) return;
  const int count = m_rows.size() + (m_headers.empty() ? 0 : 1);
  const int previousCount = rowCount();
  if (count < previousCount) {
    beginRemoveRows({}, count, previousCount - 1);
    m_layout.resize(count);
    endRemoveRows();
  }
  std::vector<Row> additions;
  additions.reserve(std::max(0, count - previousCount));
  int selectionOffset = 0;
  m_hasImages = false;
  for (int row = 0; row < count; ++row) {
    const bool header = row == 0 && !m_headers.empty();
    const auto cells = header ? m_headers : m_rows[row - (m_headers.empty() ? 0 : 1)].toList();
    auto font = m_font;
    font.setBold(header);
    qreal height = 0;
    for (const auto &cell : cells) {
      const auto html = cell.toMap().value("html").toString();
      m_hasImages |= html.contains("<img", Qt::CaseInsensitive);
      height = std::max(height, textHeight(html, font, std::max(qreal(1), m_cellWidth - 2 * m_cellPadding),
                                           m_lineHeight, m_imageCache));
    }
    Row entry{cells, height + 2 * m_cellPadding + (row + 1 < count ? 1 : 0), selectionOffset, header};
    selectionOffset += cells.size();
    if (row >= previousCount)
      additions.emplace_back(std::move(entry));
    else if (m_layout[row] != entry) {
      m_layout[row] = std::move(entry);
      emit dataChanged(index(row), index(row));
    }
  }
  if (!additions.empty()) {
    beginInsertRows({}, previousCount, count - 1);
    m_layout.reserve(count);
    for (auto &row : additions)
      m_layout.emplace_back(std::move(row));
    endInsertRows();
  }
}
