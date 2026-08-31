#include "match-highlighter.hpp"
#include "fuzzy/normalize.hpp"
#include <QTextDocument>

// case fold must stay 1:1 per UTF-16 unit so match offsets line up with the document
static QString foldForMatch(QStringView text) {
  QString out;
  out.reserve(text.size());

  for (QChar c : text) {
    if (char folded = fzf::foldDiacritic(c.unicode())) {
      out.append(QLatin1Char(folded >= 'A' && folded <= 'Z' ? folded + ('a' - 'A') : folded));
    } else {
      out.append(c.toLower());
    }
  }

  return out;
}

MatchHighlighter::MatchHighlighter(QObject *parent) : QSyntaxHighlighter(parent) {}

void MatchHighlighter::setTextDocument(QQuickTextDocument *document) {
  if (m_textDocument == document) return;

  if (m_textDocument && m_textDocument->textDocument()) {
    disconnect(m_textDocument->textDocument(), nullptr, this, nullptr);
  }

  m_textDocument = document;
  auto *doc = document ? document->textDocument() : nullptr;
  setDocument(doc);

  if (doc) {
    connect(doc, &QTextDocument::contentsChanged, this, &MatchHighlighter::updateFirstMatchPosition);
  }

  emit textDocumentChanged();
  updateFirstMatchPosition();
}

void MatchHighlighter::setTerms(const QStringList &terms) {
  if (m_terms == terms) return;

  m_terms = terms;
  m_foldedTerms.clear();
  for (const auto &term : terms) {
    if (!term.isEmpty()) m_foldedTerms << foldForMatch(term);
  }

  emit termsChanged();
  if (document()) rehighlight();
  updateFirstMatchPosition();
}

void MatchHighlighter::setColor(const QColor &color) {
  if (m_color == color) return;

  m_color = color;
  emit colorChanged();
  if (document() && !m_foldedTerms.isEmpty()) rehighlight();
}

void MatchHighlighter::highlightBlock(const QString &text) {
  if (m_foldedTerms.isEmpty()) return;

  QTextCharFormat fmt;
  fmt.setBackground(m_color);

  QString const folded = foldForMatch(text);

  for (const QString &term : m_foldedTerms) {
    for (qsizetype from = 0; (from = folded.indexOf(term, from)) != -1; from += term.size()) {
      setFormat(from, term.size(), fmt);
    }
  }
}

void MatchHighlighter::updateFirstMatchPosition() {
  int position = -1;

  if (auto *doc = document(); doc && !m_foldedTerms.isEmpty()) {
    QString const folded = foldForMatch(doc->toRawText());

    for (const QString &term : m_foldedTerms) {
      auto idx = folded.indexOf(term);
      if (idx != -1 && (position == -1 || idx < position)) position = static_cast<int>(idx);
    }
  }

  if (position == m_firstMatchPosition) return;

  m_firstMatchPosition = position;
  emit firstMatchPositionChanged();
}
