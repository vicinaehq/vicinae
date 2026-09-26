#include <QClipboard>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QtConcurrentRun>
#include <algorithm>
#include <utility>
#include "markdown-model.hpp"
#include "markdown-parser.hpp"

namespace vicinae::document {

struct MarkdownModel::StyleBinding {
  QProperty<markdown::Style> value;
  QPropertyNotifier observer;
};

MarkdownModel::MarkdownModel(QObject *parent)
    : DocumentModel(parent), m_styles(std::make_unique<StyleBinding>()) {
  m_styles->observer = m_styles->value.addNotifier([this] {
    ++m_styleGeneration;
    if (!m_markdown.isEmpty()) reparse();
  });
  setStyle(DocumentStyle::defaults());
}

MarkdownModel::~MarkdownModel() = default;

void MarkdownModel::setStyle(DocumentStyle *style) {
  if (!style) style = DocumentStyle::defaults();
  if (m_style == style) return;
  if (m_style) disconnect(m_style, nullptr, this, nullptr);
  m_style = style;
  connect(style, &QObject::destroyed, this, [this] { setStyle(nullptr); });
  m_styles->value.setBinding([this] { return markdown::Style(*m_style); });
  emit styleChanged();
}

int MarkdownModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_blocks.size());
}

QVariant MarkdownModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_blocks.size())) return {};

  const auto &block = m_blocks[index.row()];
  switch (role) {
  case BlockTypeRole:
    return static_cast<int>(block.type);
  case BlockDataRole:
    return block.data;
  default:
    return {};
  }
}

QHash<int, QByteArray> MarkdownModel::roleNames() const {
  return {
      {BlockTypeRole, "blockType"},
      {BlockDataRole, "blockData"},
  };
}

void MarkdownModel::setMarkdown(const QString &markdown) {
  static const QRegularExpression MATH_FENCE(QStringLiteral("(?:`{3,}|~{3,})[ \t]*math(?:\\s|$)"));
  if (markdown.contains('$') || markdown.contains("\\(") || markdown.contains("\\[") ||
      markdown.contains(MATH_FENCE)) {
    setMarkdownAsync(markdown);
    return;
  }
  updateMarkdown(markdown, false);
}

void MarkdownModel::updateMarkdown(QString markdown, bool asynchronous) {
  if (m_markdown == markdown) return;
  m_markdown = std::move(markdown);
  m_async = asynchronous;
  reparse();
  emit markdownChanged();
}

void MarkdownModel::reparse() {
  ++m_parseGeneration;
  if (m_async) {
    setLoading(true);
    if (!m_parseRunning) startParse();
  } else {
    applyBlocks(m_markdown, markdown::parse(m_markdown, m_styles->value.value()));
    setLoading(false);
  }
}

void MarkdownModel::applyBlocks(const QString &markdown, std::vector<markdown::Block> blocks) {
  const bool incremental = !m_renderedMarkdown.isEmpty() && markdown.startsWith(m_renderedMarkdown);
  const bool appended = incremental && markdown.size() > m_renderedMarkdown.size();
  m_renderedMarkdown = markdown;
  if (incremental) {
    const int oldCount = static_cast<int>(m_blocks.size());
    const int newCount = static_cast<int>(blocks.size());
    if (appended) emit blocksAppended();

    int common = 0;
    while (common < std::min(oldCount, newCount) && m_blocks[common].type == blocks[common].type) {
      if (m_blocks[common].data != blocks[common].data) {
        m_blocks[common] = std::move(blocks[common]);
        emit dataChanged(index(common), index(common), {BlockDataRole});
      }
      ++common;
    }
    if (common < oldCount) {
      beginRemoveRows({}, common, oldCount - 1);
      m_blocks.erase(m_blocks.begin() + common, m_blocks.end());
      endRemoveRows();
    }
    if (common < newCount) {
      beginInsertRows({}, common, newCount - 1);
      m_blocks.reserve(blocks.size());
      m_blocks.insert(m_blocks.end(), std::make_move_iterator(blocks.begin() + common),
                      std::make_move_iterator(blocks.end()));
      endInsertRows();
    }
    return;
  }
  beginResetModel();
  m_blocks = std::move(blocks);
  endResetModel();
}

void MarkdownModel::setLoading(bool loading) {
  if (m_loading == loading) return;
  m_loading = loading;
  emit loadingChanged();
}

void MarkdownModel::setMarkdownAsync(QString markdown) { updateMarkdown(std::move(markdown), true); }

void MarkdownModel::startParse() {
  m_parseRunning = true;
  const auto generation = m_parseGeneration;
  const auto styleGeneration = m_styleGeneration;
  const auto markdown = m_markdown;
  QtConcurrent::run([markdown, styles = m_styles->value.value()] {
    return markdown::parse(markdown, styles);
  }).then(this, [this, generation, styleGeneration, markdown](std::vector<markdown::Block> blocks) {
    m_parseRunning = false;
    if (!m_async) return;
    if (styleGeneration == m_styleGeneration && m_markdown.startsWith(markdown))
      applyBlocks(markdown, std::move(blocks));
    if (generation != m_parseGeneration)
      startParse();
    else
      setLoading(false);
  });
}

void MarkdownModel::clear() {
  ++m_parseGeneration;
  m_async = false;
  m_renderedMarkdown.clear();
  setLoading(false);
  beginResetModel();
  m_blocks.clear();
  const bool hadMarkdown = !m_markdown.isEmpty();
  m_markdown.clear();
  endResetModel();
  if (hadMarkdown) emit markdownChanged();
}

QString MarkdownModel::copyCodeBlock(int blockIndex) {
  if (blockIndex < 0 || std::cmp_greater_equal(blockIndex, m_blocks.size())) return {};
  const auto &block = m_blocks[blockIndex];
  if (block.type != Markdown::BlockType::CodeBlock) return {};
  auto code = block.data.value(QStringLiteral("code")).toString();
  QGuiApplication::clipboard()->setText(code);
  return code;
}

std::span<const DocumentPart> MarkdownModel::documentParts(int row) const {
  const auto &block = m_blocks[row];
  if (!block.parts) block.parts = markdownDocumentParts(block.type, block.data);
  return *block.parts;
}

DocumentModel::TextSnapshot MarkdownModel::textSnapshot(int row) const {
  const auto &block = m_blocks[row];
  if (block.parts) return DocumentModel::textSnapshot(row);
  return [type = block.type, data = block.data] { return markdownDocumentParts(type, data); };
}

} // namespace vicinae::document
