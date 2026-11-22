#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>
#include <QRegularExpression>
#include <QQuickItem>
#include <QMimeData>
#include <QTextBlock>
#include <QTextCursor>
#include <algorithm>
#include <limits>
#include <unordered_set>
#include <vector>
#include "editor-document.hpp"

namespace Vicinae::Editor {

void EditorDocument::setDropHandler(std::function<bool(const QMimeData *)> accepts,
                                    std::function<bool(const QMimeData *, int)> insert) {
  m_acceptDrop = std::move(accepts);
  m_drop = std::move(insert);
}

bool EditorDocument::acceptsDrop(const QMimeData *mime) const {
  return mime && (m_acceptDrop ? m_acceptDrop(mime) : mime->hasText());
}

bool EditorDocument::drop(const QMimeData *mime, int position) {
  if (!validRange(position, position) || !acceptsDrop(mime)) return false;
  if (m_drop && m_drop(mime, position)) return true;
  if (!mime->hasText()) return false;
  QTextCursor cursor(m_document);
  cursor.setPosition(position);
  cursor.beginEditBlock();
  cursor.insertText(mime->text());
  cursor.endEditBlock();
  emit selectionRequested(cursor.position(), cursor.position());
  return true;
}

class MarkdownHighlighter : public KSyntaxHighlighting::SyntaxHighlighter {
public:
  MarkdownHighlighter(const KSyntaxHighlighting::Repository &repository, QObject *parent)
      : SyntaxHighlighter(parent) {
    setDefinition(repository.definitionForName(QStringLiteral("Markdown")));
    const auto formats = definition().formats();
    m_markdownFormats.reserve(formats.size());
    for (const auto &format : formats)
      m_markdownFormats.emplace(format.id());
    setPalette({});
  }

  void setPalette(const Syntax::Palette &palette) {
    m_palette = palette;
    m_styles = palette.styles();
    m_marker = {};
    m_marker.setForeground(palette.readable(palette.marker));
    m_emphasis = {};
    m_emphasis.setForeground(palette.readable(palette.emphasis));
    m_heading = palette.readable(palette.heading);
    m_link = palette.readable(palette.link);
    m_linkTarget = palette.readable(palette.linkTarget);
    m_inlineCode = palette.readable(palette.inlineCode);
    rehighlight();
  }

protected:
  void highlightBlock(const QString &text) override {
    m_line = text;
    SyntaxHighlighter::highlightBlock(text);
    m_line = {};
  }

  void applyFormat(int offset, int length, const KSyntaxHighlighting::Format &format) override {
    if (!format.isValid() || length <= 0) return;
    QTextCharFormat style;
    style.setForeground(m_styles[format.textStyle()]);
    style.setFontWeight(format.isBold({}) ? QFont::Bold : QFont::Normal);
    style.setFontItalic(format.isItalic({}));
    style.setFontStrikeOut(format.isStrikeThrough({}));
    if (!m_markdownFormats.contains(format.id())) {
      setFormat(offset, length, style);
      return;
    }

    const auto name = format.name();
    const auto text = m_line.mid(offset, length);
    auto delimiters = [&](QStringView characters, const QTextCharFormat &marker) {
      int start = 0;
      int end = length;
      while (start < end && characters.contains(text[start]))
        ++start;
      while (end > start && characters.contains(text[end - 1]))
        --end;
      setFormat(offset, start, marker);
      setFormat(offset + end, length - end, marker);
    };

    if (name.startsWith(u"Header H")) {
      style.setForeground(m_heading);
      setFormat(offset, length, style);
      delimiters(u"#= ", m_marker);
    } else if (name.contains(u"Emphasis Text") || name.contains(u"Strong Text") ||
               name.contains(u"Strikethrough Text")) {
      style.setForeground(m_palette.foreground);
      setFormat(offset, length, style);
      delimiters(u"*_~", m_emphasis);
    } else if (name == u"Inline Image" || name == u"Reference Image") {
      style.setForeground(m_linkTarget);
      setFormat(offset, length, style);
      if (text.startsWith(u"![")) {
        int closing = 2;
        while (closing < length && text[closing] != u']') {
          if (text[closing] == u'\\') ++closing;
          ++closing;
        }
        setFormat(offset, 2, m_marker);
        if (closing < length) {
          QTextCharFormat label;
          label.setForeground(m_link);
          setFormat(offset + 2, closing - 2, label);
          setFormat(offset + closing, std::min(2, length - closing), m_marker);
        }
      }
      delimiters(u"[]()", m_marker);
    } else if (name.contains(u"Link") || name == u"Email" || name == u"Footnote") {
      const bool destination = (name.startsWith(u"Reference-Link") && name != u"Reference-Link Name") ||
                               name == u"Inline Image: Link";
      style.setForeground(destination ? m_linkTarget : m_link);
      setFormat(offset, length, style);
      delimiters(u"[]()<>", m_marker);
    } else if (name == u"Code") {
      const bool inlineCode = text.startsWith(u'`') && text.endsWith(u'`');
      style.setForeground(inlineCode ? m_inlineCode : m_palette.foreground);
      if (inlineCode) style.setBackground(Syntax::mix(m_palette.background, m_palette.foreground, 0.06));
      setFormat(offset, length, style);
      if (inlineCode) delimiters(u"`", m_marker);
    } else if (name == u"Fenced Code" || name == u"Table" || name == u"Horizontal Rule" || name == u"List" ||
               name == u"Number List" || name == u"Blockquote" || name == u"Line Break") {
      setFormat(offset, length, m_marker);
    } else if (name.startsWith(u"Blockquote:") || name.contains(u"Normal Text")) {
      style.setForeground(m_palette.foreground);
      setFormat(offset, length, style);
    } else if (name == u"List: Checkbox") {
      style.setForeground(m_link);
      setFormat(offset, length, style);
    } else if (name.endsWith(u"Highlight Text")) {
      style.setForeground(m_palette.foreground);
      style.setBackground(Syntax::mix(m_palette.background, m_link, 0.15));
      setFormat(offset, length, style);
      delimiters(u"=", m_emphasis);
    } else {
      setFormat(offset, length, style);
    }
  }

private:
  std::unordered_set<int> m_markdownFormats;
  Syntax::Palette m_palette;
  Syntax::StyleMap m_styles;
  QTextCharFormat m_marker;
  QTextCharFormat m_emphasis;
  QColor m_heading;
  QColor m_link;
  QColor m_linkTarget;
  QColor m_inlineCode;
  QStringView m_line;
};

EditorDocument::EditorDocument(const KSyntaxHighlighting::Repository &repository, QObject *parent)
    : QObject(parent), m_highlighter(new MarkdownHighlighter(repository, this)) {}

void EditorDocument::setPalette(const Syntax::Palette &palette) { m_highlighter->setPalette(palette); }

void EditorDocument::setTextDocument(QQuickTextDocument *document) {
  if (m_quickDocument == document) return;
  m_quickDocument = document;
  // Loading through QTextDocument bypasses TextEdit::setText's viewport optimization.
  if (auto *item = document ? qobject_cast<QQuickItem *>(document->parent()) : nullptr)
    item->setFlag(QQuickItem::ItemObservesViewport);
  attach(document ? document->textDocument() : nullptr);
  emit textDocumentChanged();
}

void EditorDocument::attach(QTextDocument *document) {
  if (m_document == document) return;
  if (m_document) {
    m_pendingText = text();
    disconnect(m_document, nullptr, this, nullptr);
  }
  m_document = document;
  // Qt only reports incremental text changes once a document layout exists.
  if (document) document->documentLayout();
  m_highlighter->setDocument(document);
  if (!document) return;
  load(m_id, m_pendingText);
  connect(document, &QTextDocument::contentsChange, this, [this](int, int removed, int added) {
    if (m_loading || (!removed && !added)) return;
    ++m_revision;
    emit textChanged();
  });
}

void EditorDocument::load(const QString &id, const QString &text) {
  m_loading = true;
  m_id = id;
  ++m_generation;
  m_revision = 0;
  m_pendingText = text;
  if (m_document) {
    m_document->setUndoRedoEnabled(false);
    m_document->setPlainText(text);
    m_document->setUndoRedoEnabled(true);
  }
  m_loading = false;
  emit documentChanged();
}

QString EditorDocument::text() const {
  if (!m_document) return m_pendingText;
  // toPlainText() would also replace non-breaking spaces with regular ones.
  auto text = m_document->toRawText();
  text.replace(QChar::ParagraphSeparator, u'\n').replace(QChar::LineSeparator, u'\n');
  return text;
}

bool EditorDocument::validRange(int start, int end) const {
  return m_document && start >= 0 && end >= start && end < m_document->characterCount();
}

QVariantMap EditorDocument::snapshot(int start, int end) const {
  if (!validRange(start, end)) return {};
  QTextCursor cursor(m_document);
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  return {{"documentId", m_id},
          {"generation", m_generation},
          {"revision", m_revision},
          {"start", start},
          {"end", end},
          {"text", cursor.selectedText().replace(QChar::ParagraphSeparator, '\n')}};
}

bool EditorDocument::replace(const QVariantMap &snapshot, const QString &replacement) {
  if (!snapshot.contains("start") || !snapshot.contains("end") ||
      snapshot.value("documentId").toString() != m_id ||
      snapshot.value("generation").toInt() != m_generation ||
      snapshot.value("revision").toInt() != m_revision)
    return false;
  const auto start = snapshot.value("start").toInt();
  const auto end = snapshot.value("end").toInt();
  if (!validRange(start, end)) return false;
  QTextCursor cursor(m_document);
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  cursor.beginEditBlock();
  cursor.insertText(replacement);
  cursor.endEditBlock();
  emit selectionRequested(start, start + replacement.size());
  return true;
}

void EditorDocument::wrap(int start, int end, const QString &marker) {
  if (marker.isEmpty()) return;
  const auto selection = snapshot(start, end);
  if (selection.isEmpty()) return;
  const auto selected = selection.value("text").toString();
  const auto size = static_cast<int>(marker.size());
  const auto fill = marker.front();
  // "**word**" is bold only and "***word***" is both, so the whole run decides whether a marker applies.
  const auto applied = [size](int run) { return size == 1 ? run % 2 == 1 : run >= size; };
  int before = 0;
  int after = 0;
  while (m_document->characterAt(start - before - 1) == fill)
    ++before;
  while (m_document->characterAt(end + after) == fill)
    ++after;
  if (applied(std::min(before, after))) {
    replace(snapshot(start - size, end + size), selected);
    return;
  }
  int leading = 0;
  int trailing = 0;
  while (leading < selected.size() && selected.at(leading) == fill)
    ++leading;
  while (trailing < selected.size() - leading && selected.at(selected.size() - trailing - 1) == fill)
    ++trailing;
  if (applied(std::min(leading, trailing))) {
    replace(selection, selected.mid(size, selected.size() - size * 2));
  } else if (replace(selection, marker + selected + marker)) {
    emit selectionRequested(start + size, end + size);
  }
}

void EditorDocument::format(int start, int end, Format format) {
  if (!validRange(start, end)) return;
  if (format >= Bold && format <= Link && start != end) {
    while (start < end && m_document->characterAt(start).isSpace())
      ++start;
    while (end > start && m_document->characterAt(end - 1).isSpace())
      --end;
    if (start == end) return;
  }
  switch (format) {
  case Bold:
    return wrap(start, end, QStringLiteral("**"));
  case Italic:
    return wrap(start, end, QStringLiteral("*"));
  case StrikeThrough:
    return wrap(start, end, QStringLiteral("~~"));
  case InlineCode: {
    const auto selection = snapshot(start, end);
    const auto selected = selection.value("text").toString();
    int opening = 0;
    int closing = 0;
    while (opening < selected.size() && selected.at(opening) == '`')
      ++opening;
    while (closing < selected.size() && selected.at(selected.size() - closing - 1) == '`')
      ++closing;
    if (opening > 0 && opening == closing && selected.size() > opening + closing) {
      auto content = selected.mid(opening, selected.size() - opening - closing);
      if (content.startsWith(' ') && content.endsWith(' ') && !content.trimmed().isEmpty())
        content = content.mid(1, content.size() - 2);
      replace(selection, content);
      return;
    }
    static const QRegularExpression TICKS(QStringLiteral("`+"));
    int length = 1;
    for (const auto &match : TICKS.globalMatch(selected))
      length = std::max(length, static_cast<int>(match.capturedLength()) + 1);
    const bool surroundedByPadding =
        start > 1 && end + 1 < m_document->characterCount() - 1 &&
        m_document->characterAt(start - 1) == ' ' && m_document->characterAt(end) == ' ' &&
        m_document->characterAt(start - 2) == '`' && m_document->characterAt(end + 1) == '`';
    const auto contentStart = start - (surroundedByPadding ? 1 : 0);
    const auto contentEnd = end + (surroundedByPadding ? 1 : 0);
    if (contentStart > 0 && contentEnd < m_document->characterCount() - 1 &&
        m_document->characterAt(contentStart - 1) == '`' && m_document->characterAt(contentEnd) == '`') {
      int before = contentStart;
      int after = contentEnd;
      while (before > 0 && m_document->characterAt(before - 1) == '`')
        --before;
      while (after < m_document->characterCount() - 1 && m_document->characterAt(after) == '`')
        ++after;
      if (contentStart - before == after - contentEnd) {
        replace(snapshot(before, after), selected);
        return;
      }
    }
    const auto marker = QString(length, '`');
    const bool padded = selected.startsWith('`') || selected.endsWith('`');
    const auto padding = padded ? QStringLiteral(" ") : QString{};
    if (replace(selection, marker + padding + selected + padding + marker))
      emit selectionRequested(start + length + padding.size(), end + length + padding.size());
    return;
  }
  case Link: {
    const auto selection = snapshot(start, end);
    auto label = selection.value("text").toString();
    label.replace('\\', QStringLiteral("\\\\"));
    label.replace('[', QStringLiteral("\\["));
    label.replace(']', QStringLiteral("\\]"));
    const auto replacement = '[' + label + QStringLiteral("](https://)");
    if (replace(selection, replacement))
      emit selectionRequested(start + label.size() + 3, start + replacement.size() - 1);
    return;
  }
  case CodeBlock:
    return formatCodeBlock(start, end);
  default:
    return formatLines(start, end, format);
  }
}

void EditorDocument::formatLines(int start, int end, Format format) {
  const bool heading = format >= Heading1 && format <= Heading6;
  if (!heading && format != BulletList && format != NumberedList && format != TaskList && format != Quote)
    return;
  static const QRegularExpression PREFIX(
      R"(^([\t ]*)(?:(#{1,6})[\t ]+|([-+*]|\d+[.)])[\t ]+(\[[ xX]\][\t ]+)?|(>)[\t ]?)?)");
  static const QRegularExpression QUOTE_PREFIX(R"(^([\t ]*)(>)[\t ]?)");
  struct Line {
    QTextBlock block;
    int indent;
    int prefix;
    bool matches;
  };
  const auto first = m_document->findBlock(start);
  const auto last = m_document->findBlock(end > start ? end - 1 : end);
  std::vector<Line> lines;
  lines.reserve(last.blockNumber() - first.blockNumber() + 1);
  bool remove = true;
  for (auto block = first; block.isValid(); block = block.next()) {
    const auto text = block.text();
    if (!text.trimmed().isEmpty()) {
      const auto match = PREFIX.match(text);
      const auto quote = QUOTE_PREFIX.match(text);
      const auto marker = match.captured(3);
      const bool numbered = !marker.isEmpty() && marker.front().isDigit();
      const bool matches = heading              ? match.capturedLength(2) == format - Heading1 + 1
                           : format == Quote    ? quote.hasMatch()
                           : format == TaskList ? !match.captured(4).isEmpty()
                           : format == NumberedList
                               ? numbered && match.captured(4).isEmpty()
                               : !marker.isEmpty() && !numbered && match.captured(4).isEmpty();
      remove &= matches;
      const auto prefixLength = format == Quote
                                    ? (quote.hasMatch() ? quote.capturedLength() : match.capturedLength(1))
                                    : match.capturedLength();
      lines.emplace_back(block, match.capturedLength(1), prefixLength, matches);
    }
    if (block == last) break;
  }
  QTextCursor selection(m_document);
  selection.setPosition(start);
  selection.setPosition(end, QTextCursor::KeepAnchor);
  QTextCursor cursor(m_document);
  cursor.beginEditBlock();
  int number = 1;
  for (const auto &line : lines) {
    QString marker;
    if (!remove) {
      if (heading)
        marker = QString(format - Heading1 + 1, '#') + ' ';
      else if (format == Quote)
        marker = QStringLiteral("> ");
      else if (format == TaskList)
        marker = QStringLiteral("- [ ] ");
      else if (format == NumberedList)
        marker = QString::number(number++) + QStringLiteral(". ");
      else
        marker = QStringLiteral("- ");
      if (line.matches && format != NumberedList) continue;
    }
    cursor.setPosition(line.block.position() + line.indent);
    cursor.setPosition(line.block.position() + line.prefix, QTextCursor::KeepAnchor);
    cursor.insertText(marker);
  }
  cursor.endEditBlock();
  emit selectionRequested(selection.selectionStart(), selection.selectionEnd());
}

void EditorDocument::formatCodeBlock(int start, int end) {
  auto first = m_document->findBlock(start);
  auto last = m_document->findBlock(end > start ? end - 1 : end);
  static const QRegularExpression FENCE(R"(^(`{3,}|~{3,})[^`~]*$)");
  auto opening = FENCE.match(first.text());
  if (!opening.hasMatch() && first.previous().isValid() && last.next().isValid()) {
    opening = FENCE.match(first.previous().text());
    if (opening.hasMatch() && last.next().text().trimmed() == opening.captured(1)) {
      first = first.previous();
      last = last.next();
    } else {
      opening = {};
    }
  }
  const auto range = snapshot(first.position(), last.position() + last.text().size());
  const auto selected = range.value("text").toString();
  if (opening.hasMatch() && first != last && last.text().trimmed() == opening.captured(1)) {
    replace(range, selected.mid(first.text().size() + 1,
                                selected.size() - first.text().size() - last.text().size() - 2));
    return;
  }
  static const QRegularExpression TICKS(QStringLiteral("`+"));
  int length = 3;
  for (const auto &match : TICKS.globalMatch(selected))
    length = std::max(length, static_cast<int>(match.capturedLength()) + 1);
  const auto fence = QString(length, '`');
  const auto contentStart = first.position() + length + 1;
  if (replace(range, fence + '\n' + selected + '\n' + fence))
    emit selectionRequested(contentStart, contentStart + selected.size());
}

int EditorDocument::continueLine(int position) {
  if (!validRange(position, position)) return -1;
  QTextCursor cursor(m_document);
  cursor.setPosition(position);
  const auto block = cursor.block();
  const auto prefix = block.text().left(position - block.position());
  static const QRegularExpression LIST(R"(^(\s*)([-+*]|\d+[.)])(\s+)(\[[ xX]\]\s+)?(.*)$)");
  const auto match = LIST.match(prefix);
  QString continuation;
  cursor.beginEditBlock();
  if (match.hasMatch()) {
    if (match.captured(5).isEmpty()) {
      cursor.setPosition(block.position());
      cursor.setPosition(position, QTextCursor::KeepAnchor);
      cursor.removeSelectedText();
      cursor.endEditBlock();
      return cursor.position();
    }
    auto marker = match.captured(2);
    bool numbered = false;
    const auto number = marker.chopped(1).toLongLong(&numbered);
    if (numbered && number < std::numeric_limits<qlonglong>::max())
      marker = QString::number(number + 1) + marker.back();
    continuation = match.captured(1) + marker + match.captured(3);
    if (!match.captured(4).isEmpty()) continuation += QStringLiteral("[ ] ");
  } else {
    static const QRegularExpression INDENT(R"(^[\t ]*)");
    continuation = INDENT.match(prefix).captured();
  }
  cursor.insertText('\n' + continuation);
  cursor.endEditBlock();
  return cursor.position();
}

QVariantMap EditorDocument::find(const QString &query, int from, bool backwards) const {
  if (!m_document || query.isEmpty()) return {};
  const auto flags = backwards ? QTextDocument::FindBackward : QTextDocument::FindFlags{};
  auto cursor = m_document->find(query, from, flags);
  if (cursor.isNull())
    cursor = m_document->find(query, backwards ? m_document->characterCount() - 1 : 0, flags);
  return cursor.isNull() ? QVariantMap{}
                         : QVariantMap{{"start", cursor.selectionStart()}, {"end", cursor.selectionEnd()}};
}

} // namespace Vicinae::Editor
