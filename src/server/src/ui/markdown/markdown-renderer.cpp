#include "ui/markdown/markdown-renderer.hpp"
#include "service-registry.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include "ui/image/http-image-loader.hpp"
#include "ui/image/image.hpp"
#include "ui/image/local-image-loader.hpp"
#include "ui/image/data-uri-image-loader.hpp"
#include "ui/scroll-bar/scroll-bar.hpp"
#include "services/app-service/app-service.hpp"
#include <cmark-gfm.h>
#include <cmark-gfm-core-extensions.h>
#include <cmark-gfm-extension_api.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
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
#include <qtextbrowser.h>
#include <qtextcursor.h>
#include <qtextdocument.h>
#include <qtextformat.h>
#include <qtextlist.h>
#include <qtexttable.h>
#include <qurl.h>
#include <qurlquery.h>
#include "config/config.hpp"
#include "services/asset-resolver/asset-resolver.hpp"

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

void MarkdownRenderer::insertImage(cmark_node *node) {
  static std::vector<const char *> widthAttributes = {"raycast-width", "omnicast-width"};
  static std::vector<const char *> heightAttributes = {"raycast-height", "omnicast-height"};
  static std::vector<const char *> tintAttributes = {"raycast-colorTint", "omnicast-colorTint"};

  const char *p = cmark_node_get_url(node);
  QUrl url(p);
  QUrlQuery query(url);
  auto documentMargin = _document->documentMargin();
  int widthOffset = documentMargin * 4;
  QSize iconSize(0, 0);

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

  insertImageFromUrl(url, iconSize);
}

void MarkdownRenderer::insertImageFromUrl(const QUrl &url, const QSize &iconSize) {
  std::unique_ptr<AbstractImageLoader> imageLoader;

  if (url.scheme() == "https" || url.scheme() == "http") {
    imageLoader = std::make_unique<HttpImageLoader>(url);
  } else if (url.scheme() == "data") {
    imageLoader = std::make_unique<DataUriImageLoader>(url.toString());
  } else if (url.scheme() == "file") {
    std::filesystem::path path = url.host().isEmpty()
                                     ? url.path().toStdString()
                                     : QString("%1%2").arg(url.host()).arg(url.path()).toStdString();
    imageLoader = std::make_unique<LocalImageLoader>(path);
  } else {
    // Resolve relative image paths
    std::filesystem::path path = QString("%1%2").arg(url.host()).arg(url.path()).toStdString();
    if (auto resolved = RelativeAssetResolver::instance()->resolve(path)) { path = *resolved; }
    imageLoader = std::make_unique<LocalImageLoader>(path);
  }

  QString loaderName = QUuid::createUuid().toString(QUuid::WithoutBraces);

  // Save current cursor position for image resizing after load
  auto pos = _cursor.position();

  // Create an image
  _cursor.insertImage(loaderName);

  m_imageLoaders[loaderName] = std::move(imageLoader);
  auto *loader = m_imageLoaders[loaderName].get();
  connect(loader, &AbstractImageLoader::dataUpdated, this,
          [this, loaderName, pos, iconSize](const QPixmap &pix) {
            auto old = _cursor.position();

            // Select the placeholder image
            _cursor.setPosition(pos);
            _cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);

            QSize finalSize = iconSize;

            // Calculate size maintaining aspect ratio based on the loaded pixmap
            if (iconSize.width() > 0 && iconSize.height() == 0) {
              // Only width specified, calculate height
              qreal aspectRatio = static_cast<qreal>(pix.width()) / pix.height();
              finalSize.setHeight(static_cast<int>(iconSize.width() / aspectRatio));
            } else if (iconSize.height() > 0 && iconSize.width() == 0) {
              // Only height specified, calculate width
              qreal aspectRatio = static_cast<qreal>(pix.width()) / pix.height();
              finalSize.setWidth(static_cast<int>(iconSize.height() * aspectRatio));
            } else if (iconSize.width() == 0 && iconSize.height() == 0) {
              // No size specified, use original image size
              finalSize = QSize(pix.width(), pix.height());
            }

            // Downscale the image to fit within the viewport width while preserving aspect ratio
            qreal docMargin = _document->documentMargin();
            int availableWidth = _textEdit->viewport()->width() - static_cast<int>(2 * docMargin);
            if (availableWidth > 0 && finalSize.width() > availableWidth) {
              qreal scale = static_cast<qreal>(availableWidth) / static_cast<qreal>(finalSize.width());
              finalSize.setWidth(availableWidth);
              finalSize.setHeight(static_cast<int>(finalSize.height() * scale));
            }

            // Set image size without forcing a percentage-based maximum width
            QTextImageFormat imageFormat = _cursor.charFormat().toImageFormat();
            imageFormat.setWidth(finalSize.width());
            imageFormat.setHeight(finalSize.height());
            _cursor.setCharFormat(imageFormat);

            // Set block alignment to center
            QTextBlockFormat blockFormat = _cursor.blockFormat();
            blockFormat.setAlignment(Qt::AlignCenter);
            _cursor.setBlockFormat(blockFormat);

            _document->addResource(QTextDocument::ImageResource, loaderName, pix);

            _cursor.clearSelection();
            _cursor.setPosition(old);
          });

  loader->render({.size = QSize(INT_MAX, INT_MAX), .devicePixelRatio = devicePixelRatio()});
}

void MarkdownRenderer::insertCodeBlock(cmark_node *node, bool isClosing) {
  OmniPainter painter;
  QTextFrameFormat format;
  QTextCharFormat fontFormat;

  fontFormat.setFontFamilies({"monospace"});
  fontFormat.setFontPointSize(_basePointSize * 0.95);

  format.setBorder(2);
  format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
  format.setBorderBrush(painter.resolveColor(SemanticColor::SecondaryBackgroundBorder));
  format.setBackground(painter.resolveColor(SemanticColor::SecondaryBackground));
  format.setPadding(10);
  format.setTopMargin(15);
  format.setBottomMargin(15);

  QString code = cmark_node_get_literal(node);
  auto frame = _cursor.insertFrame(format);

  if (isClosing && code.size() > 0) {
    while (code.at(code.size() - 1).isSpace()) {
      code.remove(code.size() - 1, 1);
    }
  }

  _cursor.insertText(code.trimmed(), fontFormat);
  _cursor.setPosition(frame->lastPosition());
  _cursor.movePosition(QTextCursor::NextCharacter);
}

void MarkdownRenderer::insertTable(cmark_node *node) {
  // Ensure we are on a new block before inserting the table
  insertIfNotFirstBlock();

  // Identify header/body sections if present
  cmark_node *header = nullptr;
  cmark_node *body = nullptr;
  std::vector<cmark_node *> extraBodyRows;
  for (auto *child = cmark_node_first_child(node); child; child = cmark_node_next(child)) {
    switch (getGfmNodeType(child)) {
    case GfmNodeType::TableHeader:
      header = child;
      break;
    case GfmNodeType::TableBody:
      body = child;
      break;
    case GfmNodeType::TableRow:
      extraBodyRows.push_back(child);
      break;
    default:
      break;
    }
  }

  // Get the number of columns
  int columnCount = cmark_gfm_extensions_get_table_columns(node);
  if (columnCount <= 0) { return; }

  // Counts the number of rows from either header or body
  auto countRowsFromSection = [&](cmark_node *section) {
    if (!section) return 0;
    auto *first = cmark_node_first_child(section);
    if (!first) return 0;
    if (getGfmNodeType(first) == GfmNodeType::TableCell) {
      return 1; // single row represented by direct cells
    }
    int rows = 0;
    for (auto *row = first; row; row = cmark_node_next(row)) {
      if (getGfmNodeType(row) == GfmNodeType::TableRow) { ++rows; }
    }
    return rows;
  };

  // Counts the total number of rows (header, body, and rows)
  int headerRows = countRowsFromSection(header);
  int bodyRows = countRowsFromSection(body);
  int extraRows = static_cast<int>(extraBodyRows.size());
  int rowCount = headerRows + bodyRows + extraRows;

  // Create QTextTable with styling
  QTextTableFormat tableFormat;
  tableFormat.setCellPadding(8);
  tableFormat.setCellSpacing(0);
  tableFormat.setBorder(1);
  tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
  tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);

  QTextTable *table = _cursor.insertTable(rowCount, columnCount, tableFormat);

  // Determine per-column alignment (default to left)
  // https://github.com/github/cmark-gfm/blob/587a12bb54d95ac37241377e6ddc93ea0e45439b/extensions/table.c#L589-L592
  std::vector<Qt::Alignment> columnAlignment(columnCount, Qt::AlignLeft);
  if (auto *alignments = cmark_gfm_extensions_get_table_alignments(node)) {
    for (int c = 0; c < columnCount; ++c) {
      switch (alignments[c]) {
      case 'l':
        columnAlignment[c] = Qt::AlignLeft;
        break;
      case 'c':
        columnAlignment[c] = Qt::AlignHCenter;
        break;
      case 'r':
        columnAlignment[c] = Qt::AlignRight;
        break;
      default:
        columnAlignment[c] = Qt::AlignLeft;
        break;
      }
    }
  }

  // Text Formatting
  OmniPainter painter;
  QTextCharFormat defaultTextFormat;
  defaultTextFormat.setFont(_document->defaultFont());
  defaultTextFormat.setForeground(painter.resolveColor(SemanticColor::Foreground));
  QTextCharFormat headerTextFormat = defaultTextFormat;
  headerTextFormat.setFontWeight(QFont::DemiBold);

  // Cell Formatting
  QTextCharFormat headerCellFormat;
  headerCellFormat.setBackground(painter.resolveColor(SemanticColor::SecondaryBackground));

  // Renders all inline/block content inside a table cell
  auto renderCellContents = [&](cmark_node *cell, bool isHeader) {
    QTextCharFormat fmt = isHeader ? headerTextFormat : defaultTextFormat;
    for (auto *content = cmark_node_first_child(cell); content; content = cmark_node_next(content)) {
      switch (cmark_node_get_type(content)) {
      case CMARK_NODE_PARAGRAPH:
        insertParagraph(content);
        break;
      case CMARK_NODE_IMAGE:
        insertImage(content);
        break;
      case CMARK_NODE_HTML_INLINE: {
        QString html = cmark_node_get_literal(content);
        insertHtmlBlock(html);
        break;
      }
      default:
        insertSpan(content, fmt);
        break;
      }
    }
  };

  // Renders a list of cells for a given row
  auto renderCellsList = [&](cmark_node *firstCell, int rowIdx, bool isHeader) {
    int c = 0;
    for (auto *cell = firstCell; cell && c < columnCount; cell = cmark_node_next(cell)) {
      if (getGfmNodeType(cell) != GfmNodeType::TableCell) { continue; }
      QTextTableCell qtCell = table->cellAt(rowIdx, c);
      if (isHeader) { qtCell.setFormat(headerCellFormat); }
      QTextCursor prevCursor = _cursor;
      _cursor = qtCell.firstCursorPosition();
      // Apply column alignment
      {
        QTextBlockFormat bf = _cursor.blockFormat();
        bf.setAlignment(columnAlignment[c]);
        _cursor.setBlockFormat(bf);
      }
      renderCellContents(cell, isHeader);
      _cursor = prevCursor;
      ++c;
    }
  };

  // Renders a section of the table (header or body)
  auto renderSection = [&](cmark_node *section, bool isHeader, int &rowIndex) {
    if (!section) return;
    auto *first = cmark_node_first_child(section);
    if (!first) return;

    // Direct table_cell children: render a single row
    if (getGfmNodeType(first) == GfmNodeType::TableCell) {
      renderCellsList(first, rowIndex, isHeader);
      ++rowIndex;
      return;
    }

    // table_row children: render each row
    for (auto *row = first; row; row = cmark_node_next(row)) {
      if (getGfmNodeType(row) != GfmNodeType::TableRow) { continue; }
      renderCellsList(cmark_node_first_child(row), rowIndex, isHeader);
      ++rowIndex;
    }
  };

  int rowIndex = 0;
  renderSection(header, true, rowIndex);
  renderSection(body, false, rowIndex);
  // Render any table_row children directly under the table node as body rows
  for (auto *row : extraBodyRows) {
    renderCellsList(cmark_node_first_child(row), rowIndex, false);
    ++rowIndex;
  }

  // Ensure subsequent content is inserted AFTER the table.
  // By default, after insertTable the cursor remains inside the first cell.
  _cursor.setPosition(table->lastPosition());
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
        if (i > 0) _cursor.insertBlock();
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
    fmt.setForeground(painter.colorBrush(SemanticColor::Red));
    fmt.setBackground(painter.colorBrush(SemanticColor::SecondaryBackground));
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
  OmniPainter painter;
  cmark_node *child = cmark_node_first_child(node);
  QTextCharFormat defaultFormat;
  size_t i = 0;

  defaultFormat.setFont(_document->defaultFont());
  defaultFormat.setForeground(painter.resolveColor(SemanticColor::Foreground));
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
    case CMARK_NODE_HTML_INLINE: {
      QString html = cmark_node_get_literal(child);
      insertHtmlBlock(html);
      break;
    }
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

void MarkdownRenderer::insertHtmlBlock(const QString &html) {
  insertIfNotFirstBlock();

  // Wrap HTML fragment in a root element for parsing
  QByteArray htmlBytes = html.toUtf8();
  QByteArray wrappedHtml = "<div>" + htmlBytes + "</div>";

  htmlDocPtr doc = htmlReadMemory(wrappedHtml.constData(), wrappedHtml.size(), nullptr, nullptr,
                                  HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
  if (!doc) { return; }

  xmlNode *root = xmlDocGetRootElement(doc);
  if (root) { processHtmlNodes(root->children); }

  xmlFreeDoc(doc);
}

void MarkdownRenderer::processHtmlNodes(xmlNode *node) {
  if (!node) return;

  for (xmlNode *cur = node; cur != nullptr; cur = cur->next) {
    // element nodes are tags
    if (cur->type == XML_ELEMENT_NODE) {
      // insert images
      if (xmlStrcmp(cur->name, BAD_CAST "img") == 0) {
        insertHtmlImage(cur);
      } else {
        // recurse inside, ignoring the tag itself
        processHtmlNodes(cur->children);
      }
      continue;
    }

    // Render text nodes as markdown
    if (cur->type == XML_TEXT_NODE) {
      xmlChar *content = xmlNodeGetContent(cur);
      if (!content) { continue; }
      QString trimmed = QString::fromUtf8(reinterpret_cast<const char *>(content)).trimmed();
      if (!trimmed.isEmpty()) {
        cmark_node *root = parseMarkdown(trimmed);
        cmark_node *node = cmark_node_first_child(root);
        while (node) {
          insertTopLevelNode(node);
          node = cmark_node_next(node);
        }
        cmark_node_free(root);
      }
      xmlFree(content);
      continue;
    }

    // ignore other element types (comments, etc.)
  }
}

void MarkdownRenderer::insertHtmlImage(xmlNode *node) {
  if (!node || xmlStrcmp(node->name, BAD_CAST "img") != 0) { return; }

  QString src;
  QSize imgSize(0, 0);

  // Extract attributes from the img node
  for (xmlAttrPtr attr = node->properties; attr != nullptr; attr = attr->next) {
    const xmlChar *attrName = attr->name;
    xmlChar *attrValue = xmlNodeListGetString(node->doc, attr->children, 1);

    if (attrValue) {
      QString name = QString::fromUtf8(reinterpret_cast<const char *>(attrName)).toLower();
      QString value = QString::fromUtf8(reinterpret_cast<const char *>(attrValue));

      if (name == "src") {
        src = value;
      } else if (name == "width") {
        if (!value.isEmpty()) { imgSize.setWidth(value.toInt()); }
      } else if (name == "height") {
        if (!value.isEmpty()) { imgSize.setHeight(value.toInt()); }
      } else if (name == "style") {
        // Support inline CSS sizing, e.g. style="width: 200px; height: 300px;"
        const auto declarations = value.split(';', Qt::SkipEmptyParts);
        for (const auto &decl : declarations) {
          const auto parts = decl.split(':', Qt::SkipEmptyParts);
          if (parts.size() != 2) continue;
          QString prop = parts[0].trimmed().toLower();
          QString val = parts[1].trimmed();

          auto parsePx = [](const QString &s, bool *okOut) -> int {
            bool ok = false;
            QString v = s.trimmed();
            if (v.endsWith("px", Qt::CaseInsensitive)) {
              v.chop(2);
              v = v.trimmed();
            }
            int res = v.toInt(&ok);
            if (okOut) *okOut = ok;
            return res;
          };

          if (prop == "width") {
            bool ok = false;
            int w = parsePx(val, &ok);
            if (ok && w > 0) { imgSize.setWidth(w); }
          } else if (prop == "height") {
            bool ok = false;
            int h = parsePx(val, &ok);
            if (ok && h > 0) { imgSize.setHeight(h); }
          }
        }
      }

      xmlFree(attrValue);
    }
  }

  if (!src.isEmpty()) { insertImageFromUrl(QUrl(src), imgSize); }
}

void MarkdownRenderer::setBaseTextColor(const ColorLike &color) { m_baseTextColor = color; }

void MarkdownRenderer::insertHeading(cmark_node *node) {
  int level = cmark_node_get_heading_level(node);

  QTextBlockFormat blockFormat;
  QTextCharFormat charFormat;

  insertIfNotFirstBlock();

  blockFormat.setTopMargin(level == 1 ? 20 : 15);
  blockFormat.setBottomMargin(level == 1 ? 15 : 10);

  charFormat.setFont(_document->defaultFont());
  charFormat.setFontPointSize(getHeadingLevelPointSize(level));
  charFormat.setFontWeight(QFont::Bold);

  _cursor.setBlockFormat(blockFormat);
  _cursor.setBlockCharFormat(charFormat);

  // Render all inline children like inline code
  for (auto *child = cmark_node_first_child(node); child; child = cmark_node_next(child)) {
    QTextCharFormat childFormat = charFormat;
    insertSpan(child, childFormat);
    // Return to heading format
    _cursor.setCharFormat(charFormat);
  }
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
  case CMARK_NODE_HTML_BLOCK: {
    QString html = cmark_node_get_literal(node);
    insertHtmlBlock(html);
    break;
  }
  default:
    // Handle GFM extension nodes by type string
    // Extensions get a dynamic type enum value, so can't depend on that
    if (getGfmNodeType(node) == GfmNodeType::Table) { insertTable(node); }
    break;
  }

  _lastNodeType = type;
}

cmark_node *MarkdownRenderer::parseMarkdown(const QString &markdown) {
  auto buf = markdown.toUtf8();

  // Enable GFM core extensions (tables, etc.) and parse
  cmark_gfm_core_extensions_ensure_registered();
  cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);

  if (cmark_syntax_extension *tableExt = cmark_find_syntax_extension("table")) {
    cmark_parser_attach_syntax_extension(parser, tableExt);
  }

  cmark_parser_feed(parser, buf.data(), buf.size());
  cmark_node *root = cmark_parser_finish(parser);
  cmark_parser_free(parser);

  return root;
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

  cmark_node *root = parseMarkdown(fragment);
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

      if (_markdown.at(i) == '\n') {
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
    : _document(new QTextDocument), _textEdit(new QTextBrowser(this)),
      _basePointSize(DEFAULT_BASE_POINT_SIZE) {
  auto layout = new QVBoxLayout;

  _lastNodePosition.renderedText = 0;
  _lastNodePosition.originalMarkdown = 0;

  _document->setUseDesignMetrics(true);
  _textEdit->setReadOnly(true);
  _textEdit->setFrameShape(QFrame::NoFrame);
  _textEdit->setOpenLinks(false); // we handle this ourselves, see below.
  _textEdit->setDocument(_document);
  _textEdit->setVerticalScrollBar(new OmniScrollBar);
  _document->setDocumentMargin(10);
  _textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
  _textEdit->setTabStopDistance(40);
  //_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _textEdit->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  setFont(QApplication::font());

  auto config = ServiceRegistry::instance()->config();

  _basePointSize = config->value().font.normal.size;

  connect(config, &config::Manager::configChanged, this,
          [this, config]() { _basePointSize = config->value().font.normal.size; });
  connect(_textEdit, &QTextBrowser::anchorClicked, this, [](const QUrl &url) {
    if (!ServiceRegistry::instance()->appDb()->openTarget(url)) {
      qWarning() << "Failed to open link" << url;
    }
  });

  _cursor = QTextCursor(_document);
}
