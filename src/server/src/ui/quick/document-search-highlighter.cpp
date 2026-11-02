#include <QAbstractTextDocumentLayout>
#include <QTimer>
#include <QVariantMap>
#include <utility>
#include "document-match-geometry.hpp"
#include "document-search-highlighter.hpp"

void DocumentSearchHighlighter::setSearch(DocumentSearch *search) {
  if (m_search == search) return;
  if (m_search) disconnect(m_search, nullptr, this, nullptr);
  m_search = search;
  if (search) {
    connect(search, &DocumentSearch::resultsChanged, this, &DocumentSearchHighlighter::scheduleRefresh);
    connect(search, &DocumentSearch::currentIndexChanged, this, &DocumentSearchHighlighter::scheduleRefresh);
    connect(search, &QObject::destroyed, this, &DocumentSearchHighlighter::scheduleRefresh);
  }
  scheduleRefresh();
  emit searchChanged();
}

void DocumentSearchHighlighter::setTextDocument(QQuickTextDocument *document) {
  if (m_textDocument == document) return;
  if (m_textDocument) disconnect(m_textDocument, nullptr, this, nullptr);
  m_textDocument = document;
  if (document) {
    connect(document, &QQuickTextDocument::textDocumentChanged, this,
            &DocumentSearchHighlighter::attachDocument);
  }
  attachDocument();
  emit textDocumentChanged();
}

void DocumentSearchHighlighter::attachDocument() {
  auto *document = m_textDocument ? m_textDocument->textDocument() : nullptr;
  if (m_document == document) return;
  if (m_document) {
    disconnect(m_document, nullptr, this, nullptr);
    disconnect(m_document->documentLayout(), nullptr, this, nullptr);
  }
  m_document = document;
  if (document) {
    connect(document, &QTextDocument::contentsChanged, this, &DocumentSearchHighlighter::scheduleRefresh);
    // relayouts from a new width or a loaded image move the matches without changing the text
    connect(document->documentLayout(), &QAbstractTextDocumentLayout::update, this,
            &DocumentSearchHighlighter::scheduleRefresh);
    connect(document->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this,
            &DocumentSearchHighlighter::scheduleRefresh);
  }
  scheduleRefresh();
}

void DocumentSearchHighlighter::setRow(int row) {
  if (m_row == row) return;
  m_row = row;
  scheduleRefresh();
  emit locationChanged();
}

void DocumentSearchHighlighter::setPart(int part) {
  if (m_part == part) return;
  m_part = part;
  scheduleRefresh();
  emit locationChanged();
}

void DocumentSearchHighlighter::scheduleRefresh() {
  if (std::exchange(m_refreshScheduled, true)) return;
  QTimer::singleShot(0, this, &DocumentSearchHighlighter::refresh);
}

void DocumentSearchHighlighter::refresh() {
  m_refreshScheduled = false;

  QVariantList rects;
  if (m_search && m_document) {
    const auto *active = m_search->currentMatch();
    for (const auto &match : m_search->matches(m_row, m_part)) {
      const bool current = active && *active == match;
      for (const auto &rect : document_match::rects(*m_document, match.start, match.length)) {
        rects.append(QVariantMap{{QStringLiteral("rect"), rect}, {QStringLiteral("current"), current}});
      }
    }
  }

  if (rects == m_rects) return;
  m_rects = std::move(rects);
  emit rectsChanged();
}
