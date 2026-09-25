#pragma once

#include <QColor>
#include <optional>
#include "document-highlighter.hpp"
#include "document-search.hpp"

class DocumentSearchHighlighter : public DocumentHighlighter {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(DocumentSearch *search READ search WRITE setSearch NOTIFY searchChanged)
  Q_PROPERTY(int row READ row WRITE setRow NOTIFY locationChanged)
  Q_PROPERTY(int part READ part WRITE setPart NOTIFY locationChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY colorChanged)

signals:
  void searchChanged();
  void locationChanged();
  void colorChanged();

public:
  using DocumentHighlighter::DocumentHighlighter;
  DocumentSearch *search() const { return m_search; }
  void setSearch(DocumentSearch *search);
  int row() const { return m_row; }
  void setRow(int row);
  int part() const { return m_part; }
  void setPart(int part);
  QColor color() const { return m_color; }
  void setColor(const QColor &color);
  QColor currentColor() const { return m_currentColor; }
  void setCurrentColor(const QColor &color);

protected:
  void highlightBlock(const QString &text) override;
  void documentChanged() override { refresh(); }

private:
  void refresh();
  QPointer<DocumentSearch> m_search;
  std::vector<DocumentSearch::Match> m_matches;
  std::optional<DocumentSearch::Match> m_current;
  int m_row = -1;
  int m_part = 0;
  QColor m_color;
  QColor m_currentColor;
};
