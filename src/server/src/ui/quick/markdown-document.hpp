#pragma once

#include <QVariantMap>
#include <vector>
#include "document-model.hpp"

namespace Markdown {
Q_NAMESPACE
QML_NAMED_ELEMENT(Markdown)

enum class BlockType : int {
  Heading,
  Paragraph,
  CodeBlock,
  BulletList,
  OrderedList,
  Table,
  Image,
  HorizontalRule,
  HtmlBlock,
  Blockquote,
  Callout,
  Math,
};
Q_ENUM_NS(BlockType)
} // namespace Markdown

std::vector<DocumentPart> markdownDocumentParts(Markdown::BlockType type, const QVariantMap &data);
void indexMarkdownList(QVariantList &items, int &part);
