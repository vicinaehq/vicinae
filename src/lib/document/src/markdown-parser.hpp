#pragma once

#include <memory>
#include <optional>
#include <vector>
#include "markdown-document.hpp"
#include "math-renderer.hpp"
#include "syntax-highlighter.hpp"

namespace vicinae::document::markdown {

struct Block {
  Markdown::BlockType type;
  QVariantMap data;
  mutable std::optional<std::vector<DocumentPart>> parts;
  std::shared_ptr<const math::Resources> resources;
};

struct Style {
  Style() = default;
  explicit Style(const DocumentStyle &style);
  bool operator==(const Style &) const = default;

  QString inlineCodeBg;
  QString linkColor;
  QString textColor;
  QString monoFamily;
  syntax::StyleMap syntax;
  bool dark = false;
};

std::vector<Block> parse(const QString &markdown, const Style &style);

} // namespace vicinae::document::markdown
