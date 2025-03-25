#include "ui/markdown/markdown-renderer.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <cmark.h>
#include <qstringview.h>

int MarkdownRenderer::getHeadingLevelPointSize(int level) const {
  auto factor = HEADING_LEVEL_SCALE_FACTORS[std::clamp(level, 1, 4)];

  return _basePointSize * factor;
}

void MarkdownRenderer::insertHeading(const QString &text, int level) {
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

void MarkdownRenderer::insertImage(cmark_node *node) {
  const char *p = cmark_node_get_url(node);

  qDebug() << "Render url" << p;
}

void MarkdownRenderer::insertList(cmark_node *list) {
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
        insertParagraph(node);
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

void MarkdownRenderer::insertBlockParagraph(cmark_node *node) {
  QTextBlockFormat blockFormat;

  blockFormat.setAlignment(Qt::AlignLeft);
  blockFormat.setTopMargin(10);
  blockFormat.setBottomMargin(10);
  _cursor.setBlockFormat(blockFormat);
  insertParagraph(node);
}

void MarkdownRenderer::insertSpan(cmark_node *node, QTextCharFormat &fmt) {
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
    insertSpan(child, fmt);
    child = cmark_node_next(child);
  }
}

void MarkdownRenderer::insertParagraph(cmark_node *node) {
  cmark_node *child = cmark_node_first_child(node);
  QTextCharFormat defaultFormat;

  if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }

  defaultFormat.setFontPointSize(12);

  while (child) {
    QTextCharFormat fmt = defaultFormat;

    switch (cmark_node_get_type(child)) {
    case CMARK_NODE_IMAGE:
      insertImage(node);
      break;
    default:
      insertSpan(child, fmt);
      break;
    }

    child = cmark_node_next(child);
  }
}

void MarkdownRenderer::insertHeading(cmark_node *node) {
  auto textNode = cmark_node_first_child(node);

  if (cmark_node_get_type(textNode) != CMARK_NODE_TEXT) { return; }

  int level = cmark_node_get_heading_level(node);
  auto text = cmark_node_get_literal(textNode);

  insertHeading(text, level);
}

void MarkdownRenderer::insertTopLevelNode(cmark_node *node) {
  auto type = cmark_node_get_type(node);

  // qDebug() << "NODE=" << cmark_node_get_type_string(node);
  switch (type) {
  case CMARK_NODE_PARAGRAPH:
    insertBlockParagraph(node);
    break;
  case CMARK_NODE_HEADING:
    insertHeading(node);
    break;
  case CMARK_NODE_LIST:
    insertList(node);
    break;
  default:
    break;
  }
}

void MarkdownRenderer::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  _textEdit->setGeometry(rect().marginsAdded(contentsMargins()));
}

QTextEdit *MarkdownRenderer::textEdit() const { return _textEdit; }

QStringView MarkdownRenderer::markdown() const { return _markdown; }

void MarkdownRenderer::clear() {
  _document->clear();
  _markdown.clear();
}

void MarkdownRenderer::setBasePointSize(int pointSize) { _basePointSize = pointSize; }

void MarkdownRenderer::appendMarkdown(QStringView markdown) {
  QString fragment;
  QTextCursor userCursor = _textEdit->textCursor();

  if (!_markdown.isEmpty()) {
    int clamped = std::clamp(_lastNodePosition.originalMarkdown, 0, (int)_markdown.size() - 1);

    fragment = _markdown.sliced(clamped) + markdown.toString();
    _cursor.setPosition(_lastNodePosition.renderedText);
    _cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    _cursor.removeSelectedText();
  } else {
    fragment = markdown.toString();
  }

  qDebug() << "new" << markdown << "fragment" << fragment;

  auto buf = fragment.toUtf8();
  cmark_node *root = cmark_parse_document(buf.data(), buf.size(), 0);
  cmark_node *node = cmark_node_first_child(root);
  cmark_node *lastNode = nullptr;

  while (node) {
    lastNode = node;
    _lastNodePosition.renderedText = _cursor.position();
    insertTopLevelNode(node);
    node = cmark_node_next(node);
  }

  _markdown.append(markdown);

  if (lastNode) {
    int localLine = cmark_node_get_start_line(lastNode);
    int localColumn = cmark_node_get_start_column(lastNode);

    int l = 1, c = 1;

    for (int i = _lastNodePosition.originalMarkdown; i < _markdown.size(); ++i) {
      if (l == localLine && c == localColumn) {
        _lastNodePosition.originalMarkdown = i;
        break;
      }

      if (_markdown.at(i) == "\n") {
        ++l;
        c = 1;
      } else {
        ++c;
      }
    }
  }

  cmark_node_free(root);
  _textEdit->setTextCursor(userCursor);
}

void MarkdownRenderer::setMarkdown(QStringView markdown) {
  clear();
  appendMarkdown(markdown);
  _cursor.setPosition(0);
  _textEdit->setTextCursor(_cursor);
}

MarkdownRenderer::MarkdownRenderer()
    : _document(new QTextDocument), _textEdit(new QTextEdit(this)), _basePointSize(DEFAULT_BASE_POINT_SIZE) {
  _textEdit->setReadOnly(true);
  _textEdit->setFrameShape(QFrame::NoFrame);
  _textEdit->setDocument(_document);
  _textEdit->setVerticalScrollBar(new OmniScrollBar);
  _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  _textEdit->show();
  _cursor = QTextCursor(_document);
  _lastNodePosition.renderedText = 0;
  _lastNodePosition.originalMarkdown = 0;
}
