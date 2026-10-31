#include "ui/quick/placeholder-highlighter.hpp"
#include <cstdint>
#include <QTextDocument>

// mirrors the grammar in utils/placeholder.cpp
enum class State : std::uint8_t { Normal, Escape, Id, KeyStart, Key, ValueStart, Value, ValueQuoted };

PlaceholderHighlighter::PlaceholderHighlighter(QObject *parent) : DocumentHighlighter(parent) {}

void PlaceholderHighlighter::setColor(QColor &slot, const QColor &color) {
  if (slot == color) return;

  slot = color;
  emit colorsChanged();
  if (document()) rehighlight();
}

void PlaceholderHighlighter::highlightBlock(const QString &text) {
  using enum State;

  auto state = previousBlockState() < 0 ? Normal : static_cast<State>(previousBlockState());
  qsizetype start = 0;

  const auto paint = [&](qsizetype from, qsizetype to, const QColor &color) {
    if (to <= from || !color.isValid()) return;
    QTextCharFormat fmt;
    fmt.setForeground(color);
    setFormat(static_cast<int>(from), static_cast<int>(to - from), fmt);
  };

  for (qsizetype i = 0; i < text.size(); ++i) {
    QChar const ch = text.at(i);

    switch (state) {
    case Normal:
      if (ch == u'\\') {
        state = Escape;
      } else if (ch == u'{') {
        paint(i, i + 1, m_braceColor);
        start = i + 1;
        state = Id;
      }
      break;
    case Escape:
      state = Normal;
      break;
    case Id:
      if (!ch.isLetterOrNumber()) {
        paint(start, i, m_nameColor);
        --i;
        state = KeyStart;
      }
      break;
    case KeyStart:
      if (ch == u'}') {
        paint(i, i + 1, m_braceColor);
        state = Normal;
      } else if (!ch.isSpace()) {
        start = i--;
        state = Key;
      }
      break;
    case Key:
      if (ch == u'=') {
        paint(start, i, m_keyColor);
        paint(i, i + 1, m_braceColor);
        state = ValueStart;
      }
      break;
    case ValueStart:
      if (!ch.isSpace()) {
        start = i--;
        state = Value;
      }
      break;
    case Value:
      if (ch == u'"') {
        paint(start, i, m_stringColor);
        start = i;
        state = ValueQuoted;
      } else if (!ch.isLetterOrNumber()) {
        paint(start, i, m_stringColor);
        --i;
        state = KeyStart;
      }
      break;
    case ValueQuoted:
      if (ch == u'"') {
        paint(start, i + 1, m_stringColor);
        start = i + 1;
        state = Value;
      }
      break;
    }
  }

  switch (state) {
  case Id:
    paint(start, text.size(), m_nameColor);
    break;
  case Key:
    paint(start, text.size(), m_keyColor);
    break;
  case Value:
  case ValueQuoted:
    paint(start, text.size(), m_stringColor);
    break;
  default:
    break;
  }

  setCurrentBlockState(static_cast<int>(state));
}
