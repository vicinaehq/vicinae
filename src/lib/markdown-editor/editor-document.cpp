#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Format>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>
#include <QRegularExpression>
#include <QQuickItem>
#include <QTextBlock>
#include <QTextCursor>
#include <limits>
#include <unordered_set>
#include "editor-document.hpp"

namespace Vicinae::Editor {

class MarkdownHighlighter : public KSyntaxHighlighting::SyntaxHighlighter {
public:
  explicit MarkdownHighlighter(QObject *parent) : SyntaxHighlighter(parent) {
    setDefinition(repository().definitionForName(QStringLiteral("Markdown")));
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
    // Honor grammar emphasis, without inheriting bold keywords or colors from an unrelated theme.
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
  static KSyntaxHighlighting::Repository &repository() {
    static KSyntaxHighlighting::Repository instance;
    return instance;
  }
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

EditorDocument::EditorDocument(QObject *parent)
    : QObject(parent), m_highlighter(new MarkdownHighlighter(this)) {}

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

QString EditorDocument::text() const { return m_document ? m_document->toPlainText() : m_pendingText; }

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
  const auto selection = snapshot(start, end);
  if (selection.isEmpty()) return;
  const auto selected = selection.value("text").toString();
  const auto surrounding = snapshot(start - marker.size(), end + marker.size());
  if (!surrounding.isEmpty() && surrounding.value("text").toString() == marker + selected + marker) {
    replace(surrounding, selected);
  } else if (selected.startsWith(marker) && selected.endsWith(marker) &&
             selected.size() >= marker.size() * 2) {
    replace(selection, selected.mid(marker.size(), selected.size() - marker.size() * 2));
  } else if (replace(selection, marker + selected + marker)) {
    emit selectionRequested(start + marker.size(), end + marker.size());
  }
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
