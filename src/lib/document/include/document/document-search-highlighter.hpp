#pragma once

#include <QColor>
#include <QQuickItem>
#include <QQuickTextDocument>
#include <optional>
#include "document-search.hpp"

namespace vicinae::document {

class DocumentSearchHighlighter : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(
      QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
  Q_PROPERTY(QQuickItem *textItem READ textItem WRITE setTextItem NOTIFY textItemChanged)
  Q_PROPERTY(vicinae::document::DocumentSearch *search READ search WRITE setSearch NOTIFY searchChanged)
  Q_PROPERTY(int row READ row WRITE setRow NOTIFY locationChanged)
  Q_PROPERTY(int part READ part WRITE setPart NOTIFY locationChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY colorChanged)

signals:
  void textDocumentChanged();
  void textItemChanged();
  void searchChanged();
  void locationChanged();
  void colorChanged();

public:
  explicit DocumentSearchHighlighter(QQuickItem *parent = nullptr);
  QQuickTextDocument *textDocument() const { return m_textDocument; }
  void setTextDocument(QQuickTextDocument *document);
  QQuickItem *textItem() const { return m_textItem; }
  void setTextItem(QQuickItem *item);
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
  void updatePolish() override;
  QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *) override;
  void geometryChange(const QRectF &geometry, const QRectF &previous) override;

private:
  struct Highlight {
    QRectF rectangle;
    std::size_t match;
  };

  void attachDocument();
  void refresh();
  QPointer<QQuickTextDocument> m_textDocument;
  QPointer<QTextDocument> m_document;
  QPointer<QQuickItem> m_textItem;
  QPointer<DocumentSearch> m_search;
  std::vector<DocumentSearch::Match> m_matches;
  std::vector<Highlight> m_highlights;
  std::optional<DocumentSearch::Match> m_current;
  int m_row = -1;
  int m_part = 0;
  QColor m_color;
  QColor m_currentColor;
};

} // namespace vicinae::document
