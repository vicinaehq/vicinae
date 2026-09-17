#include "ui/quick/text-document-edit.hpp"
#include <QTextCursor>
#include <QTextDocument>

TextDocumentEdit::TextDocumentEdit(QObject *parent) : QObject(parent) {}

void TextDocumentEdit::replace(QQuickTextDocument *document, int start, int end, const QString &text) {
  auto *doc = document ? document->textDocument() : nullptr;
  if (!doc) return;

  QTextCursor cursor(doc);
  cursor.setPosition(start);
  cursor.setPosition(end, QTextCursor::KeepAnchor);
  cursor.insertText(text);
}
