#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <cmath>
#include "text-document-edit.hpp"

namespace vicinae::document {

TextDocumentEdit::TextDocumentEdit(QObject *parent) : QObject(parent) {}

void TextDocumentEdit::replace(QQuickTextDocument *document, int start, int end, const QString &text) {
  auto *doc = document ? document->textDocument() : nullptr;
  if (!doc) return;

  QTextCursor cursor(doc);
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  cursor.insertText(text);
}

void TextDocumentEdit::setLineHeight(QQuickTextDocument *document, qreal factor) {
  if (auto *text = document ? document->textDocument() : nullptr) applyLineHeight(*text, factor);
}

void TextDocumentEdit::applyLineHeight(QTextDocument &document, qreal factor) {
  if (!std::isfinite(factor) || factor <= 0) return;
  const auto height = factor * 100;
  for (auto block = document.begin(); block.isValid(); block = block.next()) {
    const auto format = block.blockFormat();
    if (format.lineHeightType() == QTextBlockFormat::ProportionalHeight &&
        qFuzzyCompare(format.lineHeight(), height))
      continue;
    if (factor == 1 && format.lineHeightType() == QTextBlockFormat::SingleHeight) continue;
    QTextCursor cursor(&document);
    cursor.select(QTextCursor::Document);
    QTextBlockFormat spacing;
    spacing.setLineHeight(height, QTextBlockFormat::ProportionalHeight);
    cursor.mergeBlockFormat(spacing);
    return;
  }
}

} // namespace vicinae::document
