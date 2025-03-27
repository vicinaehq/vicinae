#include "ui/markdown/markdown-renderer.hpp"
#include "theme.hpp"
#include "ui/omni-scroll-bar.hpp"
#include <KSyntaxHighlighting/definition.h>
#include <KSyntaxHighlighting/syntaxhighlighter.h>
#include <KSyntaxHighlighting/theme.h>
#include <KSyntaxHighlighting/repository.h>
#include <cmark.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qfontdatabase.h>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qsize.h>
#include <qstringview.h>
#include <QTextDocumentFragment>
#include <QTextList>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <qtextformat.h>
#include <qtextlist.h>
#include <x86gprintrin.h>

int MarkdownRenderer::getHeadingLevelPointSize(int level) const {
  auto factor = HEADING_LEVEL_SCALE_FACTORS[std::clamp(level, 1, 4)];

  return _basePointSize * factor;
}

void MarkdownRenderer::insertHeading(const QString &text, int level) {
  QTextBlockFormat blockFormat;
  QTextCharFormat charFormat;

  if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }

  blockFormat.setAlignment(Qt::AlignLeft);
  blockFormat.setTopMargin(level == 1 ? 15 : 10);
  blockFormat.setBottomMargin(level == 1 ? 15 : 10);

  charFormat.setFontPointSize(getHeadingLevelPointSize(level));
  charFormat.setFontWeight(QFont::Bold);

  _cursor.setBlockFormat(blockFormat);
  _cursor.setBlockCharFormat(charFormat);
  _cursor.insertText(text);
}

void MarkdownRenderer::insertImage(cmark_node *node) {
  const char *p = cmark_node_get_url(node);

  qDebug() << "Render url" << p;
}

void MarkdownRenderer::insertCodeBlock(cmark_node *node, bool isClosing) {
  auto &theme = ThemeService::instance().theme();
  QTextFrameFormat format;
  QTextCharFormat fontFormat;

  fontFormat.setFontFamilies({"monospace"});
  fontFormat.setFontPointSize(_basePointSize * 0.95);

  format.setBorder(1);
  format.setBorderBrush(theme.colors.statusBackgroundBorder);
  format.setBackground(theme.colors.statusBackground);
  format.setPadding(10);
  format.setTopMargin(15);
  format.setBottomMargin(15);

  QString code = cmark_node_get_literal(node);
  auto frame = _cursor.insertFrame(format);

  if (isClosing && code.size() > 0) {
    while (code.at(code.size() - 1).isSpace()) {
      code.removeLast();
    }
  }

  _cursor.insertText(code, fontFormat);
  _cursor.setPosition(frame->lastPosition());
  _cursor.movePosition(QTextCursor::NextCharacter);
}

QTextList *MarkdownRenderer::insertList(cmark_node *list, int indent) {
  auto *item = cmark_node_first_child(list);

  QTextListFormat listFormat;

  listFormat.setIndent(indent);

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

  _cursor.insertBlock();
  QTextCharFormat charFormat;
  charFormat.setFontWeight(QFont::Normal);
  charFormat.setFontPointSize(_basePointSize); // Set default size

  QTextBlockFormat blockFormat;
  blockFormat.setTopMargin(5);
  blockFormat.setBottomMargin(5);

  _cursor.setCharFormat(charFormat);
  _cursor.setBlockCharFormat(charFormat);
  _cursor.setBlockFormat(blockFormat);

  QTextList *textList = _cursor.createList(listFormat);

  size_t i = 0;

  while (item) {
    if (cmark_node_get_type(item) == CMARK_NODE_ITEM) {
      auto *node = cmark_node_first_child(item);

      while (node) {
        switch (cmark_node_get_type(node)) {
        case CMARK_NODE_PARAGRAPH:
          if (i > 0) { _cursor.insertBlock(blockFormat, charFormat); }
          insertParagraph(node);
          qDebug() << "inserted paragraph in list at index" << i;
          textList->add(_cursor.block());
          break;
        case CMARK_NODE_LIST: {
          insertList(node, indent + 1);
          break;
        }
        default:
          break;
        }
        node = cmark_node_next(node);
      }
    }

    ++i;
    item = cmark_node_next(item);
  }

  return textList;
}

void MarkdownRenderer::insertBlockParagraph(cmark_node *node) {
  QTextBlockFormat blockFormat;

  if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }

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
    _cursor.insertText(cmark_node_get_literal(node), fmt);
    break;
  case CMARK_NODE_EMPH:
    fmt.setFontItalic(true);
    _cursor.insertText(cmark_node_get_literal(node), fmt);
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
    _cursor.insertText(cmark_node_get_literal(node), fmt);
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

  defaultFormat.setFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
  defaultFormat.setFontPointSize(_basePointSize);

  while (child) {
    QTextCharFormat fmt = defaultFormat;

    switch (cmark_node_get_type(child)) {
    case CMARK_NODE_IMAGE:
      insertImage(child);
      break;
    default:
      insertSpan(child, fmt);
      _cursor.setCharFormat(defaultFormat);
      break;
    }

    child = cmark_node_next(child);
  }
  _cursor.setCharFormat(defaultFormat);
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
  case CMARK_NODE_CODE_BLOCK:
    insertCodeBlock(node, !!cmark_node_next(node));
    break;
  default:
    break;
  }
}

QTextEdit *MarkdownRenderer::textEdit() const { return _textEdit; }

QStringView MarkdownRenderer::markdown() const { return _markdown; }

void MarkdownRenderer::clear() {
  _document->clear();
  _markdown.clear();
}

void MarkdownRenderer::setBasePointSize(int pointSize) { _basePointSize = pointSize; }

void MarkdownRenderer::appendMarkdown(QStringView markdown) {
  QTextCursor cursor = _textEdit->textCursor();
  int selectionStart = cursor.selectionStart();
  int selectionEnd = cursor.selectionEnd();
  QString fragment;

  if (!_markdown.isEmpty()) {
    int clamped = std::clamp(_lastNodePosition.originalMarkdown, 0, (int)_markdown.size() - 1);

    _cursor.setPosition(0);

    fragment = _markdown.sliced(clamped) + markdown.toString();
    _cursor.setPosition(_lastNodePosition.renderedText);
    _cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    _cursor.removeSelectedText();
  } else {
    fragment = markdown.toString();
  }

  qDebug() << "fragment" << fragment;

  auto buf = fragment.toUtf8();
  cmark_node *root = cmark_parse_document(buf.data(), buf.size(), 0);
  cmark_node *node = cmark_node_first_child(root);
  cmark_node *lastNode = nullptr;
  std::vector<TopLevelBlock> topLevelBlocks;

  while (node) {
    topLevelBlocks.push_back({.cursorPos = _cursor.position()});
    //_lastNodePosition.renderedText = _cursor.position();
    lastNode = node;
    insertTopLevelNode(node);
    node = cmark_node_next(node);
  }

  _markdown.append(markdown);

  if (!topLevelBlocks.empty()) {
    _lastNodePosition.renderedText = topLevelBlocks.at(topLevelBlocks.size() - 1).cursorPos;
  }

  if (lastNode) {
    // do not change uncompleted block if it's only a leading digit (work around for numbered lists)
    if (_markdown.at(_markdown.size() - 1).isDigit()) {
      if (auto previous = cmark_node_previous(lastNode)) {
        if (cmark_node_get_type(previous) == CMARK_NODE_LIST) {
          lastNode = previous;
          _lastNodePosition.renderedText = topLevelBlocks.at(topLevelBlocks.size() - 2).cursorPos;
        }
      }
    }

    qDebug() << "last node is of type" << cmark_node_get_type_string(lastNode);
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

  QTextCursor newCursor = _textEdit->textCursor();

  newCursor.setPosition(selectionStart);
  newCursor.setPosition(selectionEnd, QTextCursor::KeepAnchor);
  _textEdit->setTextCursor(newCursor);

  qDebug() << "text edit height" << _textEdit->height();
}

void MarkdownRenderer::setMarkdown(QStringView markdown) {
  clear();
  appendMarkdown(markdown);
  _cursor.setPosition(0);
  _textEdit->setTextCursor(_cursor);
}

MarkdownRenderer::MarkdownRenderer()
    : _document(new QTextDocument), _textEdit(new QTextEdit(this)), _basePointSize(DEFAULT_BASE_POINT_SIZE) {
  auto layout = new QVBoxLayout;

  _textEdit->setReadOnly(true);
  _textEdit->setFrameShape(QFrame::NoFrame);
  _textEdit->setDocument(_document);
  _textEdit->setVerticalScrollBar(new OmniScrollBar);
  _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  _textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  _textEdit->setTabStopDistance(40);
  _textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(_document->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this,
          [this](const QSizeF &size) { setFixedHeight(size.height()); });

  _cursor = QTextCursor(_document);
  _lastNodePosition.renderedText = 0;
  _lastNodePosition.originalMarkdown = 0;

  // setStyleSheet("background-color: blue");

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(_textEdit, 1);

  setLayout(layout);
}
