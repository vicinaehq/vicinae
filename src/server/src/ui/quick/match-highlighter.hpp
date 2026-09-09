#pragma once
#include <QColor>
#include <QtQml/qqmlregistration.h>
#include "ui/quick/document-highlighter.hpp"

class MatchHighlighter : public DocumentHighlighter {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(QStringList terms READ terms WRITE setTerms NOTIFY termsChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(int firstMatchPosition READ firstMatchPosition NOTIFY firstMatchPositionChanged)

public:
  explicit MatchHighlighter(QObject *parent = nullptr);

  QStringList terms() const { return m_terms; }
  void setTerms(const QStringList &terms);
  QColor color() const { return m_color; }
  void setColor(const QColor &color);
  int firstMatchPosition() const { return m_firstMatchPosition; }

signals:
  void termsChanged();
  void colorChanged();
  void firstMatchPositionChanged();

protected:
  void highlightBlock(const QString &text) override;
  void documentChanged() override;

private:
  void updateFirstMatchPosition();

  QStringList m_terms;
  QStringList m_foldedTerms;
  QColor m_color;
  int m_firstMatchPosition = -1;
};
