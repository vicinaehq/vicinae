#pragma once
#include "omni-icon.hpp"
#include <QTextBlock>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <cmark.h>
#include <qstringview.h>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <qtextedit.h>
#include <qtextformat.h>
#include <qtextlist.h>
#include <qurl.h>
#include <qwidget.h>

struct TopLevelBlock {
  int cursorPos;
};

struct ImageResource {
  int cursorPos;
  OmniIcon *icon;
  QUrl name;
};

class MarkdownRenderer : public QWidget {
  constexpr static float HEADING_LEVEL_SCALE_FACTORS[5] = {2, 1.6, 1.3, 1.16, 1};
  constexpr static int DEFAULT_BASE_POINT_SIZE = 12;

  std::vector<ImageResource> m_images;
  QString _markdown;
  QTextEdit *_textEdit;
  QTextDocument *_document;
  QTextCursor _cursor;
  int _basePointSize;
  bool _isInCodeBlock = false;

  int _lastNodeType = CMARK_NODE_NONE;

  struct {
    int originalMarkdown;
    int renderedText;
  } _lastNodePosition;

  int getHeadingLevelPointSize(int level) const;

  void insertHeading(const QString &text, int level);
  void insertImage(cmark_node *node);
  QTextList *insertList(cmark_node *list, int indent = 1);
  void insertBlockParagraph(cmark_node *node);
  void insertSpan(cmark_node *node, QTextCharFormat &fmt);
  void insertParagraph(cmark_node *node);
  void insertCodeBlock(cmark_node *node, bool isClosing = false);
  void insertHeading(cmark_node *node);
  void insertTopLevelNode(cmark_node *node);

public:
  QTextEdit *textEdit() const;
  void setFont(const QFont &font);
  QStringView markdown() const;
  void clear();

  /**
   * Set the base point size of the markdown text. Headings will automatically scale
   * their size according to this base.
   * Defaults to 12.
   */
  void setBasePointSize(int pointSize);

  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);
    _textEdit->setFixedSize(event->size());
  }

  /**
   * Appends markdown text to the existing formmated markdown content.
   *
   * This function will automatically fuse the new markdown to the already formatted content by
   * parsing the last top level markdown node again. For this reason, whitespace characters are important
   * and not trimmed in any way.
   *
   * In particular, this makes this function suitable for handling streamed markdown in real time
   * or emulate a typewriting effect.
   */
  void appendMarkdown(QStringView markdown);
  void setMarkdown(QStringView markdown);

  MarkdownRenderer();
};
