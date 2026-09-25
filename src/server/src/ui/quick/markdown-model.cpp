#include <QtConcurrentRun>
#include <algorithm>
#include <mutex>
#include <QClipboard>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QUrlQuery>
#include <cmark-gfm.h>
#include <cmark-gfm-core-extensions.h>
#include <cmark-gfm-extension_api.h>
#include <pugixml/pugixml.hpp>
#include <cstring>
#include <utility>
#include "ui/quick/markdown-model.hpp"
#include "markdown-math.hpp"
#include "services/font-service/font-service.hpp"
#include "ui/image/image-url.hpp"
#include "ui/quick/syntax-highlighter.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "theme/theme-file.hpp"
#include "theme/theme.hpp"

namespace {

enum class GfmNodeType { Table, TableHeader, TableBody, TableRow, TableCell, Unknown };

GfmNodeType getGfmNodeType(cmark_node *node) {
  const char *t = cmark_node_get_type_string(node);
  if (!t) return GfmNodeType::Unknown;
  if (std::strcmp(t, "table") == 0) return GfmNodeType::Table;
  if (std::strcmp(t, "table_header") == 0) return GfmNodeType::TableHeader;
  if (std::strcmp(t, "table_body") == 0) return GfmNodeType::TableBody;
  if (std::strcmp(t, "table_row") == 0) return GfmNodeType::TableRow;
  if (std::strcmp(t, "table_cell") == 0) return GfmNodeType::TableCell;
  return GfmNodeType::Unknown;
}

bool isClosedFence(cmark_node *node, QByteArrayView markdown) {
  const auto lineAt = [markdown](int line) {
    qsizetype start = 0;
    for (int current = 1; current < line; ++current) {
      const auto next = markdown.indexOf('\n', start);
      if (next < 0) return QByteArrayView{};
      start = next + 1;
    }
    const auto end = markdown.indexOf('\n', start);
    return markdown.sliced(start, (end < 0 ? markdown.size() : end) - start).trimmed();
  };
  const auto first = lineAt(cmark_node_get_start_line(node));
  const auto last = lineAt(cmark_node_get_end_line(node));
  if (first.isEmpty() || last.size() < 3 || cmark_node_get_start_line(node) == cmark_node_get_end_line(node))
    return false;
  const auto opening =
      std::ranges::find_if(first, [first](char c) { return c != first.front(); }) - first.begin();
  return (first.front() == '`' || first.front() == '~') && last.size() >= opening &&
         std::ranges::all_of(last, [first](char c) { return c == first.front(); });
}

QString imageProviderUrl(const QString &rawUrl) {
  QUrl const url(rawUrl);
  auto const scheme = url.scheme();

  if (scheme == "icon") return rawUrl;
  if (scheme == "https" || scheme == "http") return ImageURL::http(url).toString();

  if (scheme == "data") {
    ImageURL imgUrl;
    imgUrl.setType(ImageURLType::DataURI);
    imgUrl.setName(rawUrl);
    return imgUrl.toString();
  }

  if (scheme == "file") {
    auto path = url.host().isEmpty() ? url.path() : url.host() + url.path();
    return ImageURL::local(path).toString();
  }

  if (scheme.isEmpty()) return ImageURL::local(rawUrl).toString();

  return {};
}

struct InlineContext {
  const QString &inlineCodeFg;
  const QString &inlineCodeBg;
  const QString &linkColor;
  const QString &textColor;
  const QString &monoFamily;
  math::Resources &resources;
};

QString renderInlineHtml(cmark_node *node, const InlineContext &ctx);
QString imageAltText(cmark_node *imageNode);

QString renderOneInline(cmark_node *cur, const InlineContext &ctx) {
  if (markdown_math::isMath(cur)) return markdown_math::render(cur, ctx.textColor, ctx.resources);
  QString result;

  switch (cmark_node_get_type(cur)) {
  case CMARK_NODE_TEXT:
    result += QString::fromUtf8(cmark_node_get_literal(cur)).toHtmlEscaped();
    break;

  case CMARK_NODE_SOFTBREAK:
    result += ' ';
    break;

  case CMARK_NODE_LINEBREAK:
    result += QStringLiteral("<br/>");
    break;

  case CMARK_NODE_CODE:
    result += QStringLiteral("<code style=\"color:%1;background-color:%2;"
                             "font-family:'%3',monospace;\">")
                  .arg(ctx.inlineCodeFg, ctx.inlineCodeBg, ctx.monoFamily);
    result += QString::fromUtf8(cmark_node_get_literal(cur)).toHtmlEscaped();
    result += QStringLiteral("</code>");
    break;

  case CMARK_NODE_STRONG:
    result += QStringLiteral("<b>");
    result += renderInlineHtml(cmark_node_first_child(cur), ctx);
    result += QStringLiteral("</b>");
    break;

  case CMARK_NODE_EMPH:
    result += QStringLiteral("<i>");
    result += renderInlineHtml(cmark_node_first_child(cur), ctx);
    result += QStringLiteral("</i>");
    break;

  case CMARK_NODE_LINK:
    result += QStringLiteral("<a href=\"%1\" style=\"color:%2;\">")
                  .arg(QString::fromUtf8(cmark_node_get_url(cur)).toHtmlEscaped(), ctx.linkColor);
    result += renderInlineHtml(cmark_node_first_child(cur), ctx);
    result += QStringLiteral("</a>");
    break;

  case CMARK_NODE_IMAGE: {
    const auto src = QString::fromUtf8(cmark_node_get_url(cur));
    const auto title = QString::fromUtf8(cmark_node_get_title(cur));
    result += QStringLiteral("<img src=\"%1\" alt=\"%2\" title=\"%3\"/>")
                  .arg(src.toHtmlEscaped(), imageAltText(cur).toHtmlEscaped(), title.toHtmlEscaped());
    break;
  }

  case CMARK_NODE_HTML_INLINE:
    result += QString::fromUtf8(cmark_node_get_literal(cur));
    break;

  default:
    if (std::strcmp(cmark_node_get_type_string(cur), "strikethrough") == 0) {
      result += QStringLiteral("<s>");
      result += renderInlineHtml(cmark_node_first_child(cur), ctx);
      result += QStringLiteral("</s>");
    } else {
      result += renderInlineHtml(cmark_node_first_child(cur), ctx);
    }
    break;
  }

  return result;
}

QString renderInlineHtml(cmark_node *node, const InlineContext &ctx) {
  QString result;
  for (auto *cur = node; cur; cur = cmark_node_next(cur))
    result += renderOneInline(cur, ctx);
  return result;
}

QString renderInlineChildren(cmark_node *parent, const InlineContext &ctx) {
  return renderInlineHtml(cmark_node_first_child(parent), ctx);
}

QString imageAltText(cmark_node *imageNode) {
  QString alt;
  for (auto *c = cmark_node_first_child(imageNode); c; c = cmark_node_next(c)) {
    if (cmark_node_get_type(c) == CMARK_NODE_TEXT) {
      auto *lit = cmark_node_get_literal(c);
      if (lit) alt += QString::fromUtf8(lit);
    }
  }
  return alt;
}

QVariantMap buildListItem(cmark_node *itemNode, const InlineContext &ctx);

QVariantList buildListItems(cmark_node *listNode, const InlineContext &ctx) {
  QVariantList items;
  for (auto *item = cmark_node_first_child(listNode); item; item = cmark_node_next(item)) {
    if (cmark_node_get_type(item) == CMARK_NODE_ITEM) items.append(buildListItem(item, ctx));
  }
  return items;
}

QVariantMap buildListItem(cmark_node *itemNode, const InlineContext &ctx) {
  QVariantMap entry;
  QVariantList children;

  for (auto *child = cmark_node_first_child(itemNode); child; child = cmark_node_next(child)) {
    auto type = cmark_node_get_type(child);
    if (type == CMARK_NODE_PARAGRAPH) {
      entry[QStringLiteral("html")] = renderInlineChildren(child, ctx);
    } else if (type == CMARK_NODE_LIST) {
      QVariantMap sub;
      sub[QStringLiteral("ordered")] = (cmark_node_get_list_type(child) == CMARK_ORDERED_LIST);
      sub[QStringLiteral("startNumber")] = cmark_node_get_list_start(child);
      sub[QStringLiteral("items")] = buildListItems(child, ctx);
      children.append(sub);
    }
  }

  entry[QStringLiteral("children")] = children;
  return entry;
}

void collectCellHtml(cmark_node *firstCell, int columnCount, QVariantList &out, const InlineContext &ctx) {
  int c = 0;
  for (auto *cell = firstCell; cell && c < columnCount; cell = cmark_node_next(cell)) {
    if (getGfmNodeType(cell) != GfmNodeType::TableCell) continue;
    QVariantMap cellData;
    cellData[QStringLiteral("html")] = renderInlineChildren(cell, ctx);
    out.append(cellData);
    ++c;
  }
}

QVariantMap parseImageSize(const QUrl &url) {
  QVariantMap data;
  QUrlQuery query(url);

  auto tryParam = [&](const std::vector<const char *> &names) -> int {
    for (auto *name : names) {
      auto val = query.queryItemValue(name);
      if (!val.isEmpty()) return val.toInt();
    }
    return 0;
  };

  int const w = tryParam({"raycast-width", "omnicast-width"});
  int const h = tryParam({"raycast-height", "omnicast-height"});
  if (w > 0) data[QStringLiteral("width")] = w;
  if (h > 0) data[QStringLiteral("height")] = h;
  return data;
}

QVariantMap buildImageBlock(cmark_node *imageNode) {
  QUrl const imgUrl(QString::fromUtf8(cmark_node_get_url(imageNode)));
  QVariantMap data = parseImageSize(imgUrl);
  data[QStringLiteral("src")] = imageProviderUrl(imgUrl.toString());
  data[QStringLiteral("alt")] = imageAltText(imageNode);
  return data;
}

struct HtmlBlockResult {
  QString html;
  std::vector<QVariantMap> extractedImages;
};

void processHtmlNodes(pugi::xml_node node, HtmlBlockResult &result) {
  for (auto cur = node.first_child(); cur; cur = cur.next_sibling()) {
    if (cur.type() == pugi::node_element) {
      if (std::strcmp(cur.name(), "img") == 0) {
        QString src;
        int w = 0, h = 0;
        for (auto attr = cur.first_attribute(); attr; attr = attr.next_attribute()) {
          QString const name = QString::fromUtf8(attr.name()).toLower();
          QString const value = QString::fromUtf8(attr.value());

          if (name == "src") {
            src = value;
          } else if (name == "width") {
            w = value.toInt();
          } else if (name == "height") {
            h = value.toInt();
          } else if (name == "style") {
            for (const auto &decl : value.split(';', Qt::SkipEmptyParts)) {
              auto parts = decl.split(':', Qt::SkipEmptyParts);
              if (parts.size() != 2) continue;
              QString const prop = parts[0].trimmed().toLower();
              QString pval = parts[1].trimmed();
              if (pval.endsWith("px", Qt::CaseInsensitive)) pval.chop(2);
              if (prop == "width")
                w = pval.trimmed().toInt();
              else if (prop == "height")
                h = pval.trimmed().toInt();
            }
          }
        }
        if (!src.isEmpty()) {
          QVariantMap img;
          img[QStringLiteral("src")] = imageProviderUrl(src);
          img[QStringLiteral("alt")] = QString();
          if (w > 0) img[QStringLiteral("width")] = w;
          if (h > 0) img[QStringLiteral("height")] = h;
          result.extractedImages.push_back(img);
        }
      } else {
        processHtmlNodes(cur, result);
      }
    } else if (cur.type() == pugi::node_pcdata) {
      QString const text = QString::fromUtf8(cur.value()).trimmed();
      if (!text.isEmpty()) result.html += text.toHtmlEscaped();
    }
  }
}

} // anonymous namespace

MarkdownModel::MarkdownModel(QObject *parent) : DocumentModel(parent) {
  rebuildInlineStyles();
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() {
    rebuildInlineStyles();
    if (!m_markdown.isEmpty()) {
      if (m_async)
        setMarkdownAsync(m_markdown);
      else
        setMarkdown(m_markdown);
    }
  });
}

int MarkdownModel::rowCount(const QModelIndex &parent) const {
  return parent.isValid() ? 0 : static_cast<int>(m_blocks.size());
}

QVariant MarkdownModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_blocks.size())) return {};

  const auto &block = m_blocks[index.row()];
  switch (role) {
  case BlockTypeRole:
    return static_cast<int>(block.type);
  case BlockDataRole:
    return block.data;
  default:
    return {};
  }
}

QHash<int, QByteArray> MarkdownModel::roleNames() const {
  return {
      {BlockTypeRole, "blockType"},
      {BlockDataRole, "blockData"},
  };
}

void MarkdownModel::rebuildInlineStyles() {
  ++m_styleGeneration;
  auto &theme = ThemeService::instance().theme();
  m_styles.inlineCodeFg = theme.resolve(SemanticColor::Foreground).name(QColor::HexRgb);
  auto inlineCodeBg = theme.resolve(SemanticColor::Foreground);
  inlineCodeBg.setAlphaF(0.08);
  m_styles.inlineCodeBg = inlineCodeBg.name(QColor::HexArgb);
  m_styles.linkColor = theme.resolve(SemanticColor::LinkDefault).name(QColor::HexRgb);
  m_styles.textColor = theme.resolve(SemanticColor::Foreground).name(QColor::HexRgb);
  m_styles.monoFamily = ServiceRegistry::instance()->fontService()->builtinMonoFontFamily();
  m_styles.syntax = syntax::buildStyleMap(theme);
  m_styles.dark = theme.isDark();
}

std::vector<MarkdownModel::Block> MarkdownModel::parseBlocks(const QString &markdown, const Styles &styles) {
  std::vector<Block> blocks;
  blocks.reserve(32);

  static std::once_flag extensions;
  std::call_once(extensions, [] {
    cmark_gfm_core_extensions_ensure_registered();
    markdown_math::extension();
  });
  const auto buf = markdown_math::normalizeDelimiters(markdown.toUtf8());
  cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);

  if (auto *tableExt = cmark_find_syntax_extension("table"))
    cmark_parser_attach_syntax_extension(parser, tableExt);
  if (auto *strikethroughExt = cmark_find_syntax_extension("strikethrough"))
    cmark_parser_attach_syntax_extension(parser, strikethroughExt);

  cmark_parser_attach_syntax_extension(parser, markdown_math::extension());

  cmark_parser_feed(parser, buf.data(), buf.size());
  cmark_node *root = cmark_parser_finish(parser);
  cmark_parser_free(parser);

  for (auto *node = cmark_node_first_child(root); node; node = cmark_node_next(node)) {
    const auto firstBlock = blocks.size();
    math::Resources resources;
    resources.reserve(8);
    InlineContext ctx{styles.inlineCodeFg, styles.inlineCodeBg, styles.linkColor,
                      styles.textColor,    styles.monoFamily,   resources};
    auto type = cmark_node_get_type(node);

    switch (type) {
    case CMARK_NODE_PARAGRAPH: {
      bool hasImage = false;
      for (auto *c = cmark_node_first_child(node); c; c = cmark_node_next(c)) {
        auto ct = cmark_node_get_type(c);
        if (ct == CMARK_NODE_IMAGE || markdown_math::isDisplay(c)) {
          hasImage = true;
          break;
        }
        if (ct == CMARK_NODE_LINK) {
          auto *lc = cmark_node_first_child(c);
          if (lc && !cmark_node_next(lc) && cmark_node_get_type(lc) == CMARK_NODE_IMAGE) {
            hasImage = true;
            break;
          }
        }
      }

      if (!hasImage) {
        QVariantMap data;
        data[QStringLiteral("html")] = renderInlineChildren(node, ctx);
        blocks.push_back({Markdown::BlockType::Paragraph, data});
        break;
      }

      auto flushRun = [&blocks](QString &run) {
        if (run.trimmed().isEmpty()) {
          run.clear();
          return;
        }
        QVariantMap data;
        data[QStringLiteral("html")] = run;
        blocks.push_back({Markdown::BlockType::Paragraph, data});
        run.clear();
      };

      QString run;
      for (auto *c = cmark_node_first_child(node); c; c = cmark_node_next(c)) {
        auto ct = cmark_node_get_type(c);

        if (markdown_math::isDisplay(c)) {
          flushRun(run);
          blocks.emplace_back(
              Block{Markdown::BlockType::Math,
                    {{QStringLiteral("html"), markdown_math::render(c, ctx.textColor, ctx.resources)}}});
          continue;
        }
        if (ct == CMARK_NODE_IMAGE) {
          flushRun(run);
          blocks.push_back({Markdown::BlockType::Image, buildImageBlock(c)});
          continue;
        }

        if (ct == CMARK_NODE_LINK) {
          auto *lc = cmark_node_first_child(c);
          if (lc && !cmark_node_next(lc) && cmark_node_get_type(lc) == CMARK_NODE_IMAGE) {
            flushRun(run);
            auto data = buildImageBlock(lc);
            data[QStringLiteral("link")] = QString::fromUtf8(cmark_node_get_url(c));
            blocks.push_back({Markdown::BlockType::Image, data});
            continue;
          }
        }

        run += renderOneInline(c, ctx);
      }
      flushRun(run);
      break;
    }

    case CMARK_NODE_HEADING: {
      QVariantMap data;
      data[QStringLiteral("level")] = cmark_node_get_heading_level(node);
      data[QStringLiteral("html")] = renderInlineChildren(node, ctx);
      blocks.push_back({Markdown::BlockType::Heading, data});
      break;
    }

    case CMARK_NODE_CODE_BLOCK: {
      QVariantMap data;
      QString code = QString::fromUtf8(cmark_node_get_literal(node));
      while (!code.isEmpty() && code.back().isSpace())
        code.chop(1);
      data[QStringLiteral("code")] = code;
      auto *lang = cmark_node_get_fence_info(node);
      QString const language = lang ? QString::fromUtf8(lang) : QString();
      if (language == "math" && isClosedFence(node, buf)) {
        data[QStringLiteral("html")] =
            markdown_math::render(code, "$$\n" + code + "\n$$", true, styles.textColor, ctx.resources);
        blocks.emplace_back(Block{Markdown::BlockType::Math, std::move(data)});
        break;
      }
      data[QStringLiteral("language")] = language;
      bool const isDark = styles.dark;
      data[QStringLiteral("highlightedHtml")] =
          syntax::highlight(code, language, styles.syntax, isDark, styles.monoFamily);
      blocks.push_back({Markdown::BlockType::CodeBlock, data});
      break;
    }

    case CMARK_NODE_LIST: {
      bool const ordered = (cmark_node_get_list_type(node) == CMARK_ORDERED_LIST);
      QVariantMap data;
      auto items = buildListItems(node, ctx);
      int part = 0;
      indexMarkdownList(items, part);
      data[QStringLiteral("items")] = items;
      if (ordered) {
        data[QStringLiteral("startNumber")] = cmark_node_get_list_start(node);
        blocks.push_back({Markdown::BlockType::OrderedList, data});
      } else {
        blocks.push_back({Markdown::BlockType::BulletList, data});
      }
      break;
    }

    case CMARK_NODE_THEMATIC_BREAK:
      blocks.push_back({Markdown::BlockType::HorizontalRule, {}});
      break;

    case CMARK_NODE_HTML_BLOCK: {
      QString const html = QString::fromUtf8(cmark_node_get_literal(node));
      QByteArray const wrapped = "<div>" + html.toUtf8() + "</div>";

      pugi::xml_document doc;
      auto parseResult = doc.load_buffer(wrapped.constData(), wrapped.size(),
                                         pugi::parse_default | pugi::parse_ws_pcdata_single);
      if (parseResult) {
        auto root = doc.first_child();
        HtmlBlockResult result;
        processHtmlNodes(root, result);

        for (auto &img : result.extractedImages)
          blocks.push_back({Markdown::BlockType::Image, img});

        if (!result.html.isEmpty()) {
          QVariantMap data;
          data[QStringLiteral("html")] = result.html;
          blocks.push_back({Markdown::BlockType::HtmlBlock, data});
        }
      } else {
        QVariantMap data;
        data[QStringLiteral("html")] = html;
        blocks.push_back({Markdown::BlockType::HtmlBlock, data});
      }
      break;
    }

    case CMARK_NODE_BLOCK_QUOTE: {
      auto *firstChild = cmark_node_first_child(node);
      QString calloutType;

      if (firstChild && cmark_node_get_type(firstChild) == CMARK_NODE_PARAGRAPH) {
        auto *textNode = cmark_node_first_child(firstChild);
        if (textNode && cmark_node_get_type(textNode) == CMARK_NODE_TEXT) {
          QString const text = QString::fromUtf8(cmark_node_get_literal(textNode));
          static const QRegularExpression calloutRe(
              QStringLiteral("^\\[!(NOTE|TIP|IMPORTANT|WARNING|CAUTION)\\]\\s*"));
          auto match = calloutRe.match(text);
          if (match.hasMatch()) {
            calloutType = match.captured(1).toLower();
            QString const remaining = text.mid(match.capturedLength());
            cmark_node_set_literal(textNode, remaining.toUtf8().constData());
          }
        }
      }

      QVariantList paragraphs;
      for (auto *child = cmark_node_first_child(node); child; child = cmark_node_next(child)) {
        if (cmark_node_get_type(child) == CMARK_NODE_PARAGRAPH) {
          QString const html = renderInlineChildren(child, ctx);
          if (!html.isEmpty()) paragraphs.append(html);
        }
      }

      QVariantMap data;
      data[QStringLiteral("paragraphs")] = paragraphs;

      if (!calloutType.isEmpty()) {
        data[QStringLiteral("calloutType")] = calloutType;
        blocks.push_back({Markdown::BlockType::Callout, data});
      } else {
        blocks.push_back({Markdown::BlockType::Blockquote, data});
      }
      break;
    }

    default:
      if (getGfmNodeType(node) == GfmNodeType::Table) {
        int columnCount = cmark_gfm_extensions_get_table_columns(node);
        if (columnCount <= 0) break;

        QVariantMap data;
        data[QStringLiteral("columnCount")] = columnCount;

        QVariantList alignments;
        if (auto *aligns = cmark_gfm_extensions_get_table_alignments(node)) {
          for (int c = 0; c < columnCount; ++c) {
            switch (aligns[c]) {
            case 'c':
              alignments.append(1);
              break;
            case 'r':
              alignments.append(2);
              break;
            default:
              alignments.append(0);
              break;
            }
          }
        } else {
          for (int c = 0; c < columnCount; ++c)
            alignments.append(0);
        }
        data[QStringLiteral("alignments")] = alignments;

        cmark_node *header = nullptr;
        cmark_node *body = nullptr;
        std::vector<cmark_node *> extraRows;

        for (auto *child = cmark_node_first_child(node); child; child = cmark_node_next(child)) {
          switch (getGfmNodeType(child)) {
          case GfmNodeType::TableHeader:
            header = child;
            break;
          case GfmNodeType::TableBody:
            body = child;
            break;
          case GfmNodeType::TableRow:
            extraRows.push_back(child);
            break;
          default:
            break;
          }
        }

        QVariantList headers;
        if (header) {
          auto *first = cmark_node_first_child(header);
          if (first && getGfmNodeType(first) == GfmNodeType::TableCell) {
            collectCellHtml(first, columnCount, headers, ctx);
          } else if (first && getGfmNodeType(first) == GfmNodeType::TableRow) {
            collectCellHtml(cmark_node_first_child(first), columnCount, headers, ctx);
          }
        }
        data[QStringLiteral("headers")] = headers;

        QVariantList rows;
        auto addRowsFromSection = [&](cmark_node *section) {
          if (!section) return;
          auto *first = cmark_node_first_child(section);
          if (!first) return;
          if (getGfmNodeType(first) == GfmNodeType::TableCell) {
            QVariantList row;
            collectCellHtml(first, columnCount, row, ctx);
            rows.append(QVariant(row));
          } else {
            for (auto *r = first; r; r = cmark_node_next(r)) {
              if (getGfmNodeType(r) != GfmNodeType::TableRow) continue;
              QVariantList row;
              collectCellHtml(cmark_node_first_child(r), columnCount, row, ctx);
              rows.append(QVariant(row));
            }
          }
        };
        addRowsFromSection(body);
        for (auto *row : extraRows) {
          QVariantList rowData;
          collectCellHtml(cmark_node_first_child(row), columnCount, rowData, ctx);
          rows.append(QVariant(rowData));
        }
        data[QStringLiteral("rows")] = rows;

        blocks.push_back({Markdown::BlockType::Table, data});
      }
      break;
    }
    if (!resources.empty()) {
      const auto retained = std::make_shared<const math::Resources>(std::move(resources));
      for (auto row = firstBlock; row < blocks.size(); ++row)
        blocks[row].resources = retained;
    }
  }

  cmark_node_free(root);
  return blocks;
}

void MarkdownModel::setMarkdown(const QString &markdown) {
  static const QRegularExpression MATH_FENCE(QStringLiteral("(?:`{3,}|~{3,})[ \t]*math(?:\\s|$)"));
  if (markdown.contains('$') || markdown.contains("\\(") || markdown.contains("\\[") ||
      markdown.contains(MATH_FENCE)) {
    setMarkdownAsync(markdown);
    return;
  }
  ++m_parseGeneration;
  m_async = false;
  m_markdown = markdown;
  applyBlocks(markdown, parseBlocks(markdown, m_styles));
  setLoading(false);
}

void MarkdownModel::applyBlocks(const QString &markdown, std::vector<Block> blocks) {
  const bool appended = !m_renderedMarkdown.isEmpty() && markdown.startsWith(m_renderedMarkdown);
  m_renderedMarkdown = markdown;
  if (appended) {
    const int oldCount = static_cast<int>(m_blocks.size());
    const int newCount = static_cast<int>(blocks.size());
    emit blocksAppended();

    int common = 0;
    while (common < std::min(oldCount, newCount) && m_blocks[common].type == blocks[common].type) {
      if (m_blocks[common].data != blocks[common].data) {
        m_blocks[common] = std::move(blocks[common]);
        emit dataChanged(index(common), index(common), {BlockDataRole});
      }
      ++common;
    }
    if (common < oldCount) {
      beginRemoveRows({}, common, oldCount - 1);
      m_blocks.erase(m_blocks.begin() + common, m_blocks.end());
      endRemoveRows();
    }
    if (common < newCount) {
      beginInsertRows({}, common, newCount - 1);
      m_blocks.reserve(blocks.size());
      m_blocks.insert(m_blocks.end(), std::make_move_iterator(blocks.begin() + common),
                      std::make_move_iterator(blocks.end()));
      endInsertRows();
    }
    return;
  }
  beginResetModel();
  m_blocks = std::move(blocks);
  endResetModel();
}

void MarkdownModel::setLoading(bool loading) {
  if (m_loading == loading) return;
  m_loading = loading;
  emit loadingChanged();
}

void MarkdownModel::setMarkdownAsync(QString markdown) {
  ++m_parseGeneration;
  m_async = true;
  m_markdown = std::move(markdown);
  setLoading(true);
  if (!m_parseRunning) startParse();
}

void MarkdownModel::startParse() {
  m_parseRunning = true;
  const auto generation = m_parseGeneration;
  const auto styleGeneration = m_styleGeneration;
  const auto markdown = m_markdown;
  QtConcurrent::run([markdown, styles = m_styles] {
    return parseBlocks(markdown, styles);
  }).then(this, [this, generation, styleGeneration, markdown](std::vector<Block> blocks) {
    m_parseRunning = false;
    if (!m_async) return;
    if (styleGeneration == m_styleGeneration && m_markdown.startsWith(markdown))
      applyBlocks(markdown, std::move(blocks));
    if (generation != m_parseGeneration)
      startParse();
    else
      setLoading(false);
  });
}

void MarkdownModel::clear() {
  ++m_parseGeneration;
  m_async = false;
  m_renderedMarkdown.clear();
  setLoading(false);
  beginResetModel();
  m_blocks.clear();
  m_markdown.clear();
  endResetModel();
}

void MarkdownModel::openLink(const QString &url) {
  auto *appDb = ServiceRegistry::instance()->appDb();
  if (!appDb->openTarget(url)) qWarning() << "MarkdownModel: failed to open link" << url;
}

QString MarkdownModel::copyCodeBlock(int blockIndex) {
  if (blockIndex < 0 || std::cmp_greater_equal(blockIndex, m_blocks.size())) return {};
  const auto &block = m_blocks[blockIndex];
  if (block.type != Markdown::BlockType::CodeBlock) return {};
  auto code = block.data.value(QStringLiteral("code")).toString();
  QGuiApplication::clipboard()->setText(code);
  return code;
}

std::span<const DocumentPart> MarkdownModel::documentParts(int row) const {
  const auto &block = m_blocks[row];
  if (!block.parts) block.parts = markdownDocumentParts(block.type, block.data);
  return *block.parts;
}

DocumentModel::TextSnapshot MarkdownModel::textSnapshot(int row) const {
  const auto &block = m_blocks[row];
  if (block.parts) return DocumentModel::textSnapshot(row);
  return [type = block.type, data = block.data] { return markdownDocumentParts(type, data); };
}
