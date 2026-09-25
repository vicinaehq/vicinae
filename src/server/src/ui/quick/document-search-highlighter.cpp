#include <QTextBlock>
#include <QTextDocument>
#include <algorithm>
#include "document-search-highlighter.hpp"

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
  rehighlight();
  emit colorChanged();
}

void DocumentSearchHighlighter::setCurrentColor(const QColor &color) {
  if (m_currentColor == color) return;
  m_currentColor = color;
  rehighlight();
  emit colorChanged();
}

void DocumentSearchHighlighter::refresh() {
  const auto matches = m_search ? m_search->matches(m_row, m_part) : std::span<const DocumentSearch::Match>{};
  const auto *active = m_search ? m_search->currentMatch() : nullptr;
  const auto current =
      active && active->row == m_row && active->part == m_part ? std::optional(*active) : std::nullopt;
  const bool changed = !std::ranges::equal(m_matches, matches);
  if (!changed && m_current == current) return;
  const auto previous = m_current;
  if (changed) m_matches.assign(matches.begin(), matches.end());
  m_current = current;
  if (changed) {
    rehighlight();
  } else if (document()) {
    const auto highlight = [this](const auto &match) {
      if (!match) return;
      for (auto block = document()->findBlock(match->start);
           block.isValid() && block.position() < match->start + match->length; block = block.next())
        rehighlightBlock(block);
    };
    highlight(previous);
    highlight(current);
  }
}

void DocumentSearchHighlighter::highlightBlock(const QString &text) {
  const int start = currentBlock().position();
  const int end = start + text.size();
  auto match = std::ranges::lower_bound(m_matches, start, {},
                                        [](const auto &value) { return value.start + value.length; });
  for (; match != m_matches.end() && match->start < end; ++match) {
    const int from = std::max(start, match->start);
    const int to = std::min(end, match->start + match->length);
    QTextCharFormat format;
    format.setBackground(m_current && *m_current == *match ? m_currentColor : m_color);
    setFormat(from - start, to - from, format);
  }
}
