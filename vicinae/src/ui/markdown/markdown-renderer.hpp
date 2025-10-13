#pragma once
#include "../image/url.hpp"
#include "ui/image/image.hpp"
#include <QTextBlock>
#include <qlogging.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <cmark-gfm.h>
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
  std::unique_ptr<AbstractImageLoader> icon;
  QUrl name;
};

class MarkdownRenderer : public QWidget {
  constexpr static float HEADING_LEVEL_SCALE_FACTORS[5] = {2, 1.6, 1.3, 1.16, 1};
  constexpr static int DEFAULT_BASE_POINT_SIZE = 12;

  std::vector<ImageResource> m_images;
  QString _markdown;
  QFont m_font;
  QTextEdit *_textEdit;
  QTextDocument *_document;
  QTextCursor _cursor;
  int _basePointSize;
  bool m_growAsRequired = false;
  std::optional<ColorLike> m_baseTextColor = SemanticColor::Foreground;
  bool m_isFirstBlock = true;

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

  void insertIfNotFirstBlock();

public:
  void setGrowAsRequired(bool value);
  void setDocumentMargin(int margin) { _document->setDocumentMargin(margin); }
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
    setMarkdown(QString(_markdown));
  }

  void setBaseTextColor(const ColorLike &color);

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
