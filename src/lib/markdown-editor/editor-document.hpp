#pragma once

#include <QColor>
#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include "syntax-palette.hpp"

namespace Vicinae::Editor {

class MarkdownHighlighter;

class EditorDocument : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(MarkdownEditorDocument)
  Q_PROPERTY(
      QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
  Q_PROPERTY(QString documentId READ documentId NOTIFY documentChanged)
  Q_PROPERTY(int revision READ revision NOTIFY textChanged)

public:
  Q_INVOKABLE void load(const QString &id, const QString &text);
  Q_INVOKABLE QString text() const;
  Q_INVOKABLE QVariantMap snapshot(int start, int end) const;
  Q_INVOKABLE bool replace(const QVariantMap &snapshot, const QString &replacement);
  Q_INVOKABLE void wrap(int start, int end, const QString &marker);
  Q_INVOKABLE int continueLine(int position);
  Q_INVOKABLE QVariantMap find(const QString &query, int from, bool backwards = false) const;

signals:
  void textDocumentChanged();
  void documentChanged();
  void textChanged();
  void selectionRequested(int start, int end);

public:
  explicit EditorDocument(QObject *parent = nullptr);
  QQuickTextDocument *textDocument() const { return m_quickDocument; }
  void setTextDocument(QQuickTextDocument *document);
  void attach(QTextDocument *document);
  void setPalette(const Syntax::Palette &palette);
  QString documentId() const { return m_id; }
  int revision() const { return m_revision; }

private:
  bool validRange(int start, int end) const;
  QPointer<QQuickTextDocument> m_quickDocument;
  QPointer<QTextDocument> m_document;
  MarkdownHighlighter *m_highlighter;
  QString m_id;
  QString m_pendingText;
  int m_revision = 0;
  int m_generation = 0;
  bool m_loading = false;
};

} // namespace Vicinae::Editor
