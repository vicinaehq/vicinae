#include "ui/quick/document-highlighter.hpp"
#include <QTextDocument>

DocumentHighlighter::DocumentHighlighter(QObject *parent) : QSyntaxHighlighter(parent) {}

void DocumentHighlighter::setTextDocument(QQuickTextDocument *document) {
  if (m_textDocument == document) return;

  if (m_textDocument && m_textDocument->textDocument()) {
    disconnect(m_textDocument->textDocument(), nullptr, this, nullptr);
  }

  m_textDocument = document;
  setDocument(document ? document->textDocument() : nullptr);
  emit textDocumentChanged();
  documentChanged();
}
