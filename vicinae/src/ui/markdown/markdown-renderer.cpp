#include "ui/markdown/markdown-renderer.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "ui/image/http-image-loader.hpp"
#include "ui/image/image.hpp"
#include "ui/image/local-image-loader.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include <cmark-gfm.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qfontdatabase.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qplaintextedit.h>
#include <qresource.h>
#include <qsize.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <QTextDocumentFragment>
#include <QTextList>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <qtextformat.h>
#include <qtextlist.h>
#include <qurl.h>
#include <qurlquery.h>
#include "services/config/config-service.hpp"

int MarkdownRenderer::getHeadingLevelPointSize(int level) const {
  auto factor = HEADING_LEVEL_SCALE_FACTORS[std::clamp(level, 1, 4)];

  return _basePointSize * factor;
}

void MarkdownRenderer::insertIfNotFirstBlock() {
  if (m_isFirstBlock) {
    if (!_cursor.block().text().isEmpty()) { _cursor.insertBlock(); }
    m_isFirstBlock = false;
    return;
  }
  _cursor.insertBlock();
}

void MarkdownRenderer::insertHeading(const QString &text, int level) {
  QTextBlockFormat blockFormat;
  QTextCharFormat charFormat;

  insertIfNotFirstBlock();

  // blockFormat.setAlignment(Qt::AlignLeft);
  blockFormat.setTopMargin(level == 1 ? 20 : 15);
  blockFormat.setBottomMargin(level == 1 ? 15 : 10);

  charFormat.setFont(_document->defaultFont());
  charFormat.setFontPointSize(getHeadingLevelPointSize(level));
  charFormat.setFontWeight(QFont::Bold);

  _cursor.setBlockFormat(blockFormat);
  _cursor.setBlockCharFormat(charFormat);
  _cursor.insertText(text);
}

void MarkdownRenderer::insertImage(cmark_node *node) {
  static std::vector<const char *> widthAttributes = {"raycast-width", "omnicast-width"};
  static std::vector<const char *> heightAttributes = {"raycast-height", "omnicast-height"};
  static std::vector<const char *> tintAttributes = {"raycast-colorTint", "omnicast-colorTint"};

  const char *p = cmark_node_get_url(node);
  QUrl url(p);
  QUrlQuery query(url);
  auto documentMargin = _document->documentMargin();
  int widthOffset = documentMargin * 4;
  QSize iconSize(size().width() - widthOffset, size().height() - documentMargin * 2);

  for (const auto &attr : widthAttributes) {
    if (auto value = query.queryItemValue(attr); !value.isEmpty()) {
      iconSize.setWidth(value.toInt());
      break;
    }
  }

  for (const auto &attr : heightAttributes) {
    if (auto value = query.queryItemValue(attr); !value.isEmpty()) {
      iconSize.setHeight(value.toInt());
      break;
    }
  }

  for (const auto &attr : tintAttributes) {
    // implement for tint
  }

  std::unique_ptr<AbstractImageLoader> imageLoader;

  if (url.scheme() == "https") {
    imageLoader = std::make_unique<HttpImageLoader>(url);
  } else {
    std::filesystem::path path = QString("%1%2").arg(url.host()).arg(url.path()).toStdString();

    imageLoader = std::make_unique<LocalImageLoader>(path);
  }

  auto pos = _cursor.position();

  connect(imageLoader.get(), &AbstractImageLoader::dataUpdated, this, [this, url, pos](const QPixmap &pix) {
    auto old = _cursor.position();

    _cursor.setPosition(pos);
    _document->addResource(QTextDocument::ImageResource, url, pix);

    QTextBlockFormat blockFormat = _cursor.blockFormat();

    blockFormat.setAlignment(Qt::AlignCenter);

    _cursor.setBlockFormat(blockFormat);
    _cursor.insertImage(url.toString());
    _cursor.setPosition(old);
    _document->markContentsDirty(0, _document->characterCount());
  });

  imageLoader->render({.size = iconSize, .devicePixelRatio = devicePixelRatio()});
  m_images.push_back({.cursorPos = pos, .icon = std::move(imageLoader)});
}

void MarkdownRenderer::insertCodeBlock(cmark_node *node, bool isClosing) {
  auto &theme = ThemeService::instance().theme();
  QTextFrameFormat format;
  QTextCharFormat fontFormat;

  fontFormat.setFontFamilies({"monospace"});
  fontFormat.setFontPointSize(_basePointSize * 0.95);

  format.setBorder(2);
  format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  format.setBorderBrush(theme.resolveTint(SemanticColor::StatusBorder));
  format.setBackground(theme.resolveTint(SemanticColor::StatusBackground));
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

  _cursor.insertText(code.trimmed(), fontFormat);
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

  insertIfNotFirstBlock();

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
          insertParagraph(node);
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

  insertIfNotFirstBlock();
  blockFormat.setAlignment(Qt::AlignLeft);
  blockFormat.setTopMargin(10);
  blockFormat.setBottomMargin(10);
  _cursor.setBlockFormat(blockFormat);
  insertParagraph(node);
}

void MarkdownRenderer::insertSpan(cmark_node *node, QTextCharFormat &fmt) {
  auto &theme = ThemeService::instance().theme();
  OmniPainter painter;

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
    fmt.setForeground(painter.colorBrush(theme.resolveTint(SemanticColor::Red)));
    fmt.setBackground(painter.colorBrush(theme.resolveTint(SemanticColor::StatusBackground)));
    _cursor.insertText(cmark_node_get_literal(node), fmt);
    break;
  case CMARK_NODE_LINK:
    fmt.setForeground(QBrush(theme.resolveTint(SemanticColor::LinkDefault)));
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
  auto &theme = ThemeService::instance().theme();
  OmniPainter painter;
  cmark_node *child = cmark_node_first_child(node);
  QTextCharFormat defaultFormat;
  size_t i = 0;

  defaultFormat.setFont(_document->defaultFont());
  defaultFormat.setForeground(theme.resolveTint(SemanticColor::TextPrimary));
  defaultFormat.setFontPointSize(_basePointSize);

  while (child) {
    QTextCharFormat fmt = defaultFormat;

    switch (cmark_node_get_type(child)) {
    case CMARK_NODE_IMAGE:
      // XXX - Workaround so that images renderered right below a heading do not
      // appear on the same line than the heading. Since we insert a new block for each
      // paragraph (the image being the first child of the paragraph) I'm not sure why that happens.
      if (_lastNodeType == CMARK_NODE_HEADING) insertIfNotFirstBlock();
      insertImage(child);
      break;
    default:
      insertSpan(child, fmt);
      _cursor.setCharFormat(defaultFormat);
      break;
    }

    ++i;
    child = cmark_node_next(child);
  }
  _cursor.setCharFormat(defaultFormat);
}

void MarkdownRenderer::setBaseTextColor(const ColorLike &color) { m_baseTextColor = color; }

void MarkdownRenderer::insertHeading(cmark_node *node) {
  auto textNode = cmark_node_first_child(node);

  if (cmark_node_get_type(textNode) != CMARK_NODE_TEXT) { return; }

  int level = cmark_node_get_heading_level(node);
  auto text = cmark_node_get_literal(textNode);

  insertHeading(text, level);
}

void MarkdownRenderer::insertTopLevelNode(cmark_node *node) {
  auto type = cmark_node_get_type(node);

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

  _lastNodeType = type;
}

QTextEdit *MarkdownRenderer::textEdit() const { return _textEdit; }

QStringView MarkdownRenderer::markdown() const { return _markdown; }

void MarkdownRenderer::clear() {
  _lastNodePosition.renderedText = 0;
  _lastNodePosition.originalMarkdown = 0;
  _document->clear();
  _markdown.clear();
  _document->setDefaultFont(m_font);
}

void MarkdownRenderer::setBasePointSize(int pointSize) { _basePointSize = pointSize; }

void MarkdownRenderer::appendMarkdown(QStringView markdown) {
  auto oldScroll = _textEdit->verticalScrollBar()->value();
  bool isBottomScrolled =
      _textEdit->verticalScrollBar()->value() == _textEdit->verticalScrollBar()->maximum();

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

  if (isBottomScrolled) {
    _textEdit->verticalScrollBar()->setValue(_textEdit->verticalScrollBar()->maximum());
  } else {
    _textEdit->verticalScrollBar()->setValue(oldScroll);
  }
}

void MarkdownRenderer::setMarkdown(QStringView markdown) {
  m_isFirstBlock = true;
  m_images.clear();
  clear();
  appendMarkdown(markdown);
  _cursor.setPosition(0);
  _textEdit->verticalScrollBar()->setValue(0);
  _textEdit->setTextCursor(_cursor);
  qreal height = _document->size().height();

  if (m_growAsRequired) { setFixedHeight(_document->size().height()); }
}

void MarkdownRenderer::setFont(const QFont &font) {
  _document->setDefaultFont(font);
  m_font = font;
}

void MarkdownRenderer::setGrowAsRequired(bool value) { m_growAsRequired = value; }

MarkdownRenderer::MarkdownRenderer()
    : _document(new QTextDocument), _textEdit(new QTextEdit(this)), _basePointSize(DEFAULT_BASE_POINT_SIZE) {
  auto layout = new QVBoxLayout;

  _lastNodePosition.renderedText = 0;
  _lastNodePosition.originalMarkdown = 0;

  _document->setUseDesignMetrics(true);
  _textEdit->setReadOnly(true);
  _textEdit->setFrameShape(QFrame::NoFrame);
  _textEdit->setDocument(_document);
  _textEdit->setVerticalScrollBar(new OmniScrollBar);
  _document->setDocumentMargin(10);
  _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  _textEdit->setTabStopDistance(40);
  //_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _textEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  setFont(QApplication::font());

  auto config = ServiceRegistry::instance()->config();

  _basePointSize = config->value().font.baseSize;

  connect(config, &ConfigService::configChanged, this,
          [this, config]() { _basePointSize = config->value().font.baseSize; });

  _cursor = QTextCursor(_document);
}
