#include <QTextDocumentFragment>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextImageFormat>
#include <algorithm>
#include "markdown-document.hpp"

namespace {
void appendHtml(std::vector<DocumentPart> &parts, const QString &html) {
  if (parts.size() == parts.capacity()) parts.reserve(std::max<std::size_t>(8, parts.size() * 2));
  if (!html.contains("<img", Qt::CaseInsensitive)) {
    parts.emplace_back(DocumentPart{QTextDocumentFragment::fromHtml(html).toPlainText()});
    return;
  }
  QTextDocument document;
  document.setHtml(html);
  DocumentPart part{document.toPlainText()};
  part.inlineObjects.reserve(4);
  for (auto block = document.begin(); block.isValid(); block = block.next())
    for (auto it = block.begin(); !it.atEnd(); ++it) {
      const auto fragment = it.fragment();
      if (fragment.charFormat().isImageFormat()) {
        const auto alt = fragment.charFormat().stringProperty(QTextFormat::ImageAltText);
        for (int offset = 0; offset < fragment.length(); ++offset)
          part.inlineObjects.emplace_back(DocumentPart::InlineObject{fragment.position() + offset, alt});
      }
    }
  parts.emplace_back(std::move(part));
}
void appendList(std::vector<DocumentPart> &parts, const QVariantList &items) {
  for (const auto &value : items) {
    const auto item = value.toMap();
    appendHtml(parts, item.value("html").toString());
    for (const auto &child : item.value("children").toList())
      appendList(parts, child.toMap().value("items").toList());
  }
}
} // namespace

std::vector<DocumentPart> markdownDocumentParts(Markdown::BlockType type, const QVariantMap &data) {
  std::vector<DocumentPart> parts;
  parts.reserve(1);
  switch (type) {
  case Markdown::BlockType::BulletList:
  case Markdown::BlockType::OrderedList:
    appendList(parts, data.value("items").toList());
    break;
  case Markdown::BlockType::Table:
    for (const auto &cell : data.value("headers").toList())
      appendHtml(parts, cell.toMap().value("html").toString());
    for (const auto &row : data.value("rows").toList())
      for (const auto &cell : row.toList())
        appendHtml(parts, cell.toMap().value("html").toString());
    break;
  case Markdown::BlockType::Blockquote:
  case Markdown::BlockType::Callout:
    for (const auto &paragraph : data.value("paragraphs").toList())
      appendHtml(parts, paragraph.toString());
    break;
  case Markdown::BlockType::Image:
    parts.emplace_back(DocumentPart{data.value("alt").toString(), true});
    break;
  case Markdown::BlockType::HorizontalRule:
    parts.emplace_back(DocumentPart{QStringLiteral("---"), true});
    break;
  case Markdown::BlockType::CodeBlock:
    appendHtml(parts, data.value("highlightedHtml").toString());
    break;
  default:
    appendHtml(parts, data.value("html").toString());
    break;
  }
  return parts;
}

void indexMarkdownList(QVariantList &items, int &part) {
  for (auto &value : items) {
    auto item = value.toMap();
    item.insert(QStringLiteral("selectionPart"), part++);
    auto children = item.value("children").toList();
    for (auto &child : children) {
      auto list = child.toMap();
      auto nested = list.value("items").toList();
      indexMarkdownList(nested, part);
      list.insert(QStringLiteral("items"), nested);
      child = list;
    }
    item.insert(QStringLiteral("children"), children);
    value = item;
  }
}
