#include <QAbstractTextDocumentLayout>
#include <QGlyphRun>
#include <QQuickWindow>
#include <QSGRectangleNode>
#include <QSGTransformNode>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextLayout>
#include <algorithm>
#include "document-search-highlighter.hpp"

namespace vicinae::document {

DocumentSearchHighlighter::DocumentSearchHighlighter(QQuickItem *parent) : QQuickItem(parent) {
  setFlag(ItemHasContents);
  m_highlights.reserve(16);
}

void DocumentSearchHighlighter::setTextDocument(QQuickTextDocument *document) {
  if (m_textDocument == document) return;
  if (m_textDocument) disconnect(m_textDocument, nullptr, this, nullptr);
  m_textDocument = document;
  if (document) {
    connect(document, &QQuickTextDocument::textDocumentChanged, this,
            &DocumentSearchHighlighter::attachDocument);
    connect(document, &QObject::destroyed, this, &DocumentSearchHighlighter::attachDocument);
  }
  attachDocument();
  emit textDocumentChanged();
}

void DocumentSearchHighlighter::attachDocument() {
  if (m_document) {
    disconnect(m_document, nullptr, this, nullptr);
    disconnect(m_document->documentLayout(), nullptr, this, nullptr);
  }
  m_document = m_textDocument ? m_textDocument->textDocument() : nullptr;
  if (m_document) {
    connect(m_document, &QTextDocument::contentsChanged, this, &QQuickItem::polish);
    connect(m_document->documentLayout(), &QAbstractTextDocumentLayout::update, this, &QQuickItem::polish);
  }
  polish();
}

void DocumentSearchHighlighter::setTextItem(QQuickItem *item) {
  if (m_textItem == item) return;
  m_textItem = item;
  polish();
  emit textItemChanged();
}

void DocumentSearchHighlighter::setSearch(DocumentSearch *search) {
  if (m_search == search) return;
  if (m_search) disconnect(m_search, nullptr, this, nullptr);
  m_search = search;
  if (search) {
    connect(search, &DocumentSearch::resultsChanged, this, &DocumentSearchHighlighter::refresh);
    connect(search, &DocumentSearch::currentIndexChanged, this, &DocumentSearchHighlighter::refresh);
    connect(search, &QObject::destroyed, this, &DocumentSearchHighlighter::refresh);
  }
  refresh();
  emit searchChanged();
}

void DocumentSearchHighlighter::setRow(int row) {
  if (m_row == row) return;
  m_row = row;
  refresh();
  emit locationChanged();
}

void DocumentSearchHighlighter::setPart(int part) {
  if (m_part == part) return;
  m_part = part;
  refresh();
  emit locationChanged();
}

void DocumentSearchHighlighter::setColor(const QColor &color) {
  if (m_color == color) return;
  m_color = color;
  update();
  emit colorChanged();
}

void DocumentSearchHighlighter::setCurrentColor(const QColor &color) {
  if (m_currentColor == color) return;
  m_currentColor = color;
  update();
  emit colorChanged();
}

void DocumentSearchHighlighter::refresh() {
  const auto matches = m_search ? m_search->matches(m_row, m_part) : std::span<const DocumentSearch::Match>{};
  const auto *active = m_search ? m_search->currentMatch() : nullptr;
  const auto current =
      active && active->row == m_row && active->part == m_part ? std::optional(*active) : std::nullopt;
  const bool changed = !std::ranges::equal(m_matches, matches);
  if (!changed && m_current == current) return;
  m_current = current;
  if (changed) {
    m_matches.assign(matches.begin(), matches.end());
    polish();
  } else {
    update();
  }
}

void DocumentSearchHighlighter::geometryChange(const QRectF &geometry, const QRectF &previous) {
  QQuickItem::geometryChange(geometry, previous);
  polish();
}

void DocumentSearchHighlighter::updatePolish() {
  m_highlights.clear();
  if (m_document && m_textItem && !m_matches.empty()) {
    m_textItem->ensurePolished();
    // Highlight the existing glyphs; character formats would reshape text and change wrapping.
    for (std::size_t index = 0; index < m_matches.size(); ++index) {
      const auto &match = m_matches[index];
      const int end = match.start + match.length;
      for (auto block = m_document->findBlock(match.start); block.isValid() && block.position() < end;
           block = block.next()) {
        const auto *layout = block.layout();
        const int from = std::max(0, match.start - block.position());
        const int to = std::min(block.length() - 1, end - block.position());
        const auto first = layout->lineForTextPosition(from);
        if (!first.isValid()) continue;
        for (int lineIndex = first.lineNumber(); lineIndex < layout->lineCount(); ++lineIndex) {
          const auto line = layout->lineAt(lineIndex);
          if (line.textStart() >= to) break;
          const int start = std::max(from, line.textStart());
          const int length = std::min(to, line.textStart() + line.textLength()) - start;
          QRectF cursor;
          QMetaObject::invokeMethod(m_textItem, "positionToRectangle", Q_RETURN_ARG(QRectF, cursor),
                                    Q_ARG(int, block.position() + line.textStart()));
          const qreal offset = cursor.x() - line.cursorToX(line.textStart());
          const auto firstHighlight = m_highlights.size();
          const auto append = [&](qreal x, qreal width) {
            if (width <= 0) return;
            if (m_highlights.size() == m_highlights.capacity())
              m_highlights.reserve(m_highlights.capacity() * 2);
            m_highlights.emplace_back(QRectF(x + offset, cursor.y(), width, line.height()), index);
          };
          for (const auto &run : line.glyphRuns(start, length, QTextLayout::RetrieveGlyphIndexes)) {
            const auto bounds = run.boundingRect();
            append(bounds.x(), bounds.width());
          }
          const auto text = layout->text();
          for (auto tab = text.indexOf(u'\t', start); tab >= 0 && tab < start + length;
               tab = text.indexOf(u'\t', tab + 1)) {
            const auto left = line.cursorToX(tab);
            const auto right = line.cursorToX(tab + 1);
            append(std::min(left, right), std::abs(right - left));
          }
          const auto lineHighlights = std::span(m_highlights).subspan(firstHighlight);
          std::ranges::sort(lineHighlights, {},
                            [](const auto &highlight) { return highlight.rectangle.x(); });
          auto merged = firstHighlight;
          for (const auto &highlight : lineHighlights) {
            if (merged > firstHighlight &&
                highlight.rectangle.left() <= m_highlights[merged - 1].rectangle.right())
              m_highlights[merged - 1].rectangle |= highlight.rectangle;
            else
              m_highlights[merged++] = highlight;
          }
          m_highlights.resize(merged);
        }
      }
    }
  }
  update();
}

QSGNode *DocumentSearchHighlighter::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
  if (!node) node = new QSGTransformNode;
  auto *child = node->firstChild();
  for (const auto &highlight : m_highlights) {
    auto *rectangle = child ? static_cast<QSGRectangleNode *>(child) : window()->createRectangleNode();
    if (!child) node->appendChildNode(rectangle);
    rectangle->setRect(highlight.rectangle);
    rectangle->setColor(m_current && m_matches[highlight.match] == *m_current ? m_currentColor : m_color);
    child = rectangle->nextSibling();
  }
  while (child) {
    auto *next = child->nextSibling();
    node->removeChildNode(child);
    delete child;
    child = next;
  }
  return node;
}

} // namespace vicinae::document
