#pragma once
#include <QColor>
#include <QQuickTextDocument>
#include <QSyntaxHighlighter>
#include <QtQml/qqmlregistration.h>

class MatchHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(
      QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
  Q_PROPERTY(QStringList terms READ terms WRITE setTerms NOTIFY termsChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(int firstMatchPosition READ firstMatchPosition NOTIFY firstMatchPositionChanged)

public:
  explicit MatchHighlighter(QObject *parent = nullptr);

  QQuickTextDocument *textDocument() const { return m_textDocument; }
  void setTextDocument(QQuickTextDocument *document);
  QStringList terms() const { return m_terms; }
  void setTerms(const QStringList &terms);
  QColor color() const { return m_color; }
  void setColor(const QColor &color);
  int firstMatchPosition() const { return m_firstMatchPosition; }

signals:
  void textDocumentChanged();
  void termsChanged();
  void colorChanged();
  void firstMatchPositionChanged();

protected:
  void highlightBlock(const QString &text) override;

private:
  void updateFirstMatchPosition();

  QQuickTextDocument *m_textDocument = nullptr;
  QStringList m_terms;
  QStringList m_foldedTerms;
  QColor m_color;
  int m_firstMatchPosition = -1;
};
