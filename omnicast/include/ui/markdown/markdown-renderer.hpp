#pragma once
#include "theme.hpp"
#include "ui/omni-scroll-bar.hpp"
#include "wayland-wlr-data-control-client-protocol.h"
#include <algorithm>
#include <qboxlayout.h>
#include <qcursor.h>
#include <QTextBlock>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <cmark.h>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <qtextedit.h>
#include <qtextformat.h>
#include <qwidget.h>

class MarkdownRenderer : public QWidget {
  constexpr static float HEADING_LEVEL_SCALE_FACTORS[5] = {2, 1.6, 1.3, 1.16, 1};
  constexpr static int DEFAULT_BASE_POINT_SIZE = 12;

  QString _markdown;
  QTextEdit *_textEdit;
  QTextDocument *_document;
  QTextCursor _cursor;
  int _basePointSize;

  struct {
    int originalMarkdown;
    int renderedText;
  } _lastNodePosition;

  int getHeadingLevelPointSize(int level) {
    auto factor = HEADING_LEVEL_SCALE_FACTORS[std::clamp(level, 1, 4)];

    return _basePointSize * factor;
  }

  int countLine(const QString &text) const {
    int count = 0;

    for (auto ch : text) {
      if (ch == '\n') ++count;
    }

    return count;
  }

  int getFlattenedOffset(const QString &s, int line, int col) const {
    int lineCount = 1;
    int colCount = 1;

    for (int i = 0; i != s.size(); ++i) {
      if (lineCount == line && colCount == col) return i;
      if (s.at(i) == "\n") {
        ++lineCount;
        colCount = 1;
      } else {
        ++colCount;
      }
    }

    return -1;
  }

  void renderHeading(const QString &text, int level) {
    if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }

    QTextBlockFormat blockFormat;
    QTextCharFormat charFormat;

    blockFormat.setAlignment(Qt::AlignLeft);
    blockFormat.setTopMargin(level == 1 ? 15 : 10);
    blockFormat.setTopMargin(level == 1 ? 15 : 10);

    charFormat.setFontPointSize(getHeadingLevelPointSize(level));
    charFormat.setFontWeight(QFont::Bold);

    _cursor.setBlockFormat(blockFormat);
    _cursor.insertText(text, charFormat);
    _cursor.insertBlock();
  }

  void renderImage(cmark_node *node) {
    const char *p = cmark_node_get_url(node);

    qDebug() << "Render url" << p;
  }

  void renderList(cmark_node *list) {
    auto *item = cmark_node_first_child(list);
    QTextListFormat listFormat;

    listFormat.setIndent(1);

    switch (cmark_node_get_list_type(list)) {
    case CMARK_BULLET_LIST:
      listFormat.setStyle(QTextListFormat::ListDisc);
      break;
    case CMARK_ORDERED_LIST:
      listFormat.setStyle(QTextListFormat::ListDecimal);
      break;
    default:
      break;
    }

    _cursor.insertList(listFormat);

    while (item) {
      switch (cmark_node_get_type(item)) {
      case CMARK_NODE_ITEM: {
        qDebug() << "LIST ITEM";
        auto *node = cmark_node_first_child(item);

        switch (cmark_node_get_type(node)) {
        case CMARK_NODE_PARAGRAPH:
          renderParagraph(node);
          break;
        default:
          break;
        }
        break;
      }
      default:
        qDebug() << "something else item";
        break;
      }

      item = cmark_node_next(item);
    }
  }

  void renderBlockParagraph(cmark_node *node) {
    QTextBlockFormat blockFormat;

    blockFormat.setAlignment(Qt::AlignLeft);
    blockFormat.setTopMargin(10);
    blockFormat.setBottomMargin(10);
    _cursor.setBlockFormat(blockFormat);
    renderParagraph(node);
  }

  void renderSpan(cmark_node *node, QTextCharFormat &fmt) {
    switch (cmark_node_get_type(node)) {
    case CMARK_NODE_STRONG:
      fmt.setFontWeight(QFont::DemiBold);
      break;
    case CMARK_NODE_EMPH:
      fmt.setFontItalic(true);
      break;
    case CMARK_NODE_CODE:
      fmt.setFontFamilies({"monospace"});
      fmt.setBackground(QColor(50, 50, 50));
      _cursor.insertText(cmark_node_get_literal(node), fmt);
      break;
    case CMARK_NODE_LINK:
      fmt.setForeground(QBrush(QColor(240, 240, 240)));
      fmt.setFontUnderline(true);
      fmt.setAnchor(true);
      fmt.setAnchorHref(cmark_node_get_url(node));
      break;
    case CMARK_NODE_TEXT:
      _cursor.insertText(cmark_node_get_literal(node), fmt);
      break;
    default:
      break;
    }

    cmark_node *child = cmark_node_first_child(node);

    while (child) {
      renderSpan(child, fmt);
      child = cmark_node_next(child);
    }
  }

  void renderParagraph(cmark_node *node) {
    auto &theme = ThemeService::instance().theme();
    cmark_node *child = cmark_node_first_child(node);
    QTextCharFormat defaultFormat;

    if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }

    defaultFormat.setFontPointSize(12);

    while (child) {
      QTextCharFormat fmt = defaultFormat;

      switch (cmark_node_get_type(child)) {
      case CMARK_NODE_IMAGE:
        renderImage(node);
        break;
      default:
        renderSpan(child, fmt);
        break;
      }

      child = cmark_node_next(child);
    }
  }

  void renderHeading(cmark_node *node) {
    auto textNode = cmark_node_first_child(node);

    if (cmark_node_get_type(textNode) != CMARK_NODE_TEXT) { return; }

    int level = cmark_node_get_heading_level(node);
    auto text = cmark_node_get_literal(textNode);

    renderHeading(text, level);
  }

  void renderNode(cmark_node *node) {
    auto type = cmark_node_get_type(node);

    qDebug() << "NODE=" << cmark_node_get_type_string(node);
    switch (type) {
    case CMARK_NODE_PARAGRAPH:
      renderBlockParagraph(node);
      break;
    case CMARK_NODE_HEADING:
      renderHeading(node);
      break;
    case CMARK_NODE_LIST:
      renderList(node);
      break;
    default:
      break;
    }
  }

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    _textEdit->setGeometry(rect().marginsAdded(contentsMargins()));
  }

  void insertMarkdown(const QString &markdown) {
    auto buf = markdown.toUtf8();
    cmark_node *root = cmark_parse_document(buf.data(), buf.size(), 0);
    cmark_node *node = cmark_node_first_child(root);
    cmark_node *lastNode = nullptr;

    auto lines = _markdown.split("\n");
    auto oldLineCount = qMax(1, lines.size());
    int oldColumnOffset = 0;

    if (lines.size()) { oldColumnOffset = lines.at(lines.size() - 1).size(); }

    while (node) {
      lastNode = node;
      _lastNodePosition.renderedText = _cursor.position();
      renderNode(node);
      node = cmark_node_next(node);
    }

    _markdown += markdown;

    if (lastNode) {
      int localLine = cmark_node_get_start_line(lastNode);
      int localColumn = cmark_node_get_start_column(lastNode);
      int column = localLine == 1 ? (localColumn + oldColumnOffset) : localColumn;
      int line = oldLineCount + (localLine - 1);
      int offset = getFlattenedOffset(_markdown, line, column);

      qDebug() << "line" << line << "column" << column;

      _lastNodePosition.originalMarkdown = offset;
    }

    cmark_node_free(root);
  }

public:
  QTextEdit *textEdit() const { return _textEdit; }

  const QString &markdown() const { return _markdown; }

  void clear() {
    _document->clear();
    _markdown.clear();
  }

  void setBasePointSize(int pointSize) { _basePointSize = pointSize; }

  /**
   * Append markdown to the current document.
   */
  void appendMarkdown(const QString &markdown) {
    if (_markdown.isEmpty()) {
      setMarkdown(markdown);
      return;
    }

    int clamped = std::clamp(_lastNodePosition.originalMarkdown, 0, (int)_markdown.size() - 1);
    QString fragment = _markdown.sliced(clamped) + markdown;
    QTextCursor userCursor = _textEdit->textCursor();

    _markdown.truncate(clamped);

    qDebug() << "reparsing fragment" << fragment;

    _cursor.setPosition(_lastNodePosition.renderedText);
    _cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    _cursor.removeSelectedText();

    insertMarkdown(fragment);
    _textEdit->setTextCursor(userCursor);
  }

  void setMarkdown(const QString &markdown) {
    clear();
    insertMarkdown(markdown);
    _cursor.setPosition(0);
    _textEdit->setTextCursor(_cursor);
  }

  MarkdownRenderer()
      : _document(new QTextDocument), _textEdit(new QTextEdit(this)),
        _basePointSize(DEFAULT_BASE_POINT_SIZE) {
    _textEdit->setReadOnly(true);
    _textEdit->setFrameShape(QFrame::NoFrame);
    _textEdit->setDocument(_document);
    _textEdit->setVerticalScrollBar(new OmniScrollBar);
    _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    _textEdit->show();
    _cursor = QTextCursor(_document);
  }
};
