#pragma once
#include <QQuickTextDocument>
#include <QSyntaxHighlighter>
#include <QtQml/qqmlregistration.h>

class DocumentHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("abstract")
  Q_PROPERTY(
      QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)

public:
  explicit DocumentHighlighter(QObject *parent = nullptr);

  QQuickTextDocument *textDocument() const { return m_textDocument; }
  void setTextDocument(QQuickTextDocument *document);

signals:
  void textDocumentChanged();

protected:
  virtual void documentChanged() {}

private:
  QQuickTextDocument *m_textDocument = nullptr;
};
