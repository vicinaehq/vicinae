#pragma once
#include "ui/image/image.hpp"
#include <QTextBlock>
#include <qlogging.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qstring.h>
#include <cmark-gfm.h>
#include <libxml/tree.h>
#include <qstringview.h>
#include <qtextbrowser.h>
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

  // Image loaders for async image placement
  std::map<QString, std::unique_ptr<AbstractImageLoader>> m_imageLoaders;

  QString _markdown;
  QFont m_font;
  QTextBrowser *_textEdit;
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

  void insertImage(cmark_node *node);
  void insertImageFromUrl(const QUrl &url, const QSize &iconSize);
  QTextList *insertList(cmark_node *list, int indent = 1);
  void insertBlockParagraph(cmark_node *node);
  void insertSpan(cmark_node *node, QTextCharFormat &fmt);
  void insertParagraph(cmark_node *node);
  void insertCodeBlock(cmark_node *node, bool isClosing = false);
  void insertTable(cmark_node *node);
  void insertHeading(cmark_node *node);
  void insertTopLevelNode(cmark_node *node);

  void insertHtmlBlock(const QString &html);
  void processHtmlNodes(xmlNode *node);
  void insertHtmlImage(xmlNode *node);
  cmark_node *parseMarkdown(const QString &markdown);

  void insertIfNotFirstBlock();

  enum class GfmNodeType { Table, TableHeader, TableBody, TableRow, TableCell, Unknown };

  static GfmNodeType getGfmNodeType(cmark_node *node) {
    const char *t = cmark_node_get_type_string(node);
    if (!t) return GfmNodeType::Unknown;
    if (std::strcmp(t, "table") == 0) return GfmNodeType::Table;
    if (std::strcmp(t, "table_header") == 0) return GfmNodeType::TableHeader;
    if (std::strcmp(t, "table_body") == 0) return GfmNodeType::TableBody;
    if (std::strcmp(t, "table_row") == 0) return GfmNodeType::TableRow;
    if (std::strcmp(t, "table_cell") == 0) return GfmNodeType::TableCell;
    return GfmNodeType::Unknown;
  }

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
