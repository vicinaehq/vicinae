#pragma once

#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>
#include "document-search.hpp"

/**
 * Locates the search matches of a text document so they can be painted behind it. The document itself is
 * left alone: even a background-only format splits the text into separately shaped runs, which loses
 * kerning and ligatures across them and shifts the text.
 */
class DocumentSearchHighlighter : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(DocumentSearch *search READ search WRITE setSearch NOTIFY searchChanged)
  Q_PROPERTY(
      QQuickTextDocument *textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
  Q_PROPERTY(int row READ row WRITE setRow NOTIFY locationChanged)
  Q_PROPERTY(int part READ part WRITE setPart NOTIFY locationChanged)
  Q_PROPERTY(QVariantList rects READ rects NOTIFY rectsChanged)

signals:
  void searchChanged();
  void textDocumentChanged();
  void locationChanged();
  void rectsChanged();

public:
  using QObject::QObject;
  DocumentSearch *search() const { return m_search; }
  void setSearch(DocumentSearch *search);
  QQuickTextDocument *textDocument() const { return m_textDocument; }
  void setTextDocument(QQuickTextDocument *document);
  int row() const { return m_row; }
  void setRow(int row);
  int part() const { return m_part; }
  void setPart(int part);
  QVariantList rects() const { return m_rects; }

private:
  void attachDocument();
  void scheduleRefresh();
  void refresh();

  QPointer<DocumentSearch> m_search;
  QPointer<QQuickTextDocument> m_textDocument;
  QPointer<QTextDocument> m_document;
  QVariantList m_rects;
  int m_row = -1;
  int m_part = 0;
  bool m_refreshScheduled = false;
};
