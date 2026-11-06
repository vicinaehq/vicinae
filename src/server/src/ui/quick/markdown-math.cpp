#include <algorithm>
#include <vector>
#include <cctype>
#include <cstdint>
#include <string>
#include <utility>
#include "markdown-math.hpp"
#include "math-renderer.hpp"

namespace {
cmark_node *match(cmark_syntax_extension *extension, cmark_parser *, cmark_node *, unsigned char character,
                  cmark_inline_parser *parser) {
  if (character != '$') return nullptr;
  const int offset = cmark_inline_parser_get_offset(parser);
  const bool display = cmark_inline_parser_peek_at(parser, offset + 1) == '$';
  const int delimiter = display ? 2 : 1;
  const auto peek = [parser](int position) { return cmark_inline_parser_peek_at(parser, position); };
  const int begin = offset + delimiter;
  if (!display && (!peek(begin) || std::isspace(peek(begin)))) return nullptr;

  for (int end = begin; end - begin <= 8192 && peek(end); ++end) {
    if (!display && peek(end) == '\n') break;
    if (peek(end) == '$' && (!display || peek(end + 1) == '$')) {
      if (end == begin || (!display && (std::isspace(peek(end - 1)) || std::isdigit(peek(end + 1))))) break;
      std::string source;
      source.reserve(end + delimiter - offset);
      for (int i = offset; i < end + delimiter; ++i)
        source += peek(i);
      auto *node = cmark_node_new(CMARK_NODE_CUSTOM_INLINE);
      cmark_node_set_syntax_extension(node, extension);
      cmark_node_set_string_content(node, source.c_str());
      cmark_inline_parser_set_offset(parser, end + delimiter);
      return node;
    }
    if (peek(end) == '\\') ++end;
  }
  if (!display) return nullptr;
  // Keep an unfinished display delimiter together while streaming.
  auto *literal = cmark_node_new(CMARK_NODE_TEXT);
  cmark_node_set_literal(literal, "$$");
  cmark_inline_parser_set_offset(parser, offset + 2);
  return literal;
}
} // namespace

QByteArray markdown_math::normalizeDelimiters(const QByteArray &markdown) {
  if (!markdown.contains("\\(") && !markdown.contains("\\[")) return markdown;

  // cmark handles backslash escapes before inline extensions. Protect code and links
  // with its own source ranges before normalizing the alternative math delimiters.
  auto *root = cmark_parse_document(markdown.constData(), markdown.size(), CMARK_OPT_DEFAULT);
  std::vector<qsizetype> lines;
  lines.reserve(markdown.count('\n') + 1);
  lines.emplace_back(0);
  for (qsizetype i = 0; i < markdown.size(); ++i)
    if (markdown[i] == '\n') lines.emplace_back(i + 1);
  const auto position = [&](int line, int column) {
    return line > 0 && std::cmp_less_equal(line, lines.size())
               ? std::min(markdown.size(), lines[line - 1] + std::max(0, column - 1))
               : qsizetype(0);
  };
  std::vector<std::pair<qsizetype, qsizetype>> protectedRanges;
  protectedRanges.reserve(32);
  auto *iterator = cmark_iter_new(root);
  while (cmark_iter_next(iterator) != CMARK_EVENT_DONE) {
    if (cmark_iter_get_event_type(iterator) != CMARK_EVENT_ENTER) continue;
    auto *node = cmark_iter_get_node(iterator);
    const auto type = cmark_node_get_type(node);
    if (type != CMARK_NODE_CODE && type != CMARK_NODE_CODE_BLOCK && type != CMARK_NODE_HTML_INLINE &&
        type != CMARK_NODE_HTML_BLOCK && type != CMARK_NODE_LINK && type != CMARK_NODE_IMAGE)
      continue;
    protectedRanges.emplace_back(
        position(cmark_node_get_start_line(node), cmark_node_get_start_column(node)),
        std::min(markdown.size(),
                 position(cmark_node_get_end_line(node), cmark_node_get_end_column(node)) + 1));
  }
  cmark_iter_free(iterator);
  cmark_node_free(root);
  std::ranges::sort(protectedRanges);

  QByteArray result;
  result.reserve(markdown.size());
  std::size_t range = 0;
  for (qsizetype i = 0; i < markdown.size();) {
    while (range < protectedRanges.size() && protectedRanges[range].second <= i)
      ++range;
    if (range < protectedRanges.size() && protectedRanges[range].first <= i) {
      const auto end = protectedRanges[range].second;
      result += QByteArrayView(markdown).sliced(i, end - i);
      i = end;
      continue;
    }
    if (markdown[i] == '\\' && i + 1 < markdown.size()) {
      const char next = markdown[i + 1];
      if (next == '(' || next == '[') {
        const QByteArray closer = next == '(' ? "\\)" : "\\]";
        const auto end = markdown.indexOf(closer, i + 2);
        if (end >= 0 && (range == protectedRanges.size() || protectedRanges[range].first > end) &&
            (next == '[' || !QByteArrayView(markdown).sliced(i, end - i).contains('\n'))) {
          const auto delimiter = next == '(' ? "$" : "$$";
          result += delimiter;
          result += QByteArrayView(markdown).sliced(i + 2, end - i - 2);
          result += delimiter;
          i = end + 2;
          continue;
        }
      }
      result += markdown[i++];
    }
    result += markdown[i++];
  }
  return result;
}

cmark_syntax_extension *markdown_math::extension() {
  static auto *extension = [] {
    auto *result = cmark_syntax_extension_new("math");
    cmark_syntax_extension_set_match_inline_func(result, match);
    auto *characters = cmark_llist_append(cmark_get_default_mem_allocator(), nullptr,
                                          reinterpret_cast<void *>(std::uintptr_t('$')));
    cmark_syntax_extension_set_special_inline_chars(result, characters);
    return result;
  }();
  return extension;
}

bool markdown_math::isMath(cmark_node *node) { return cmark_node_get_syntax_extension(node) == extension(); }

bool markdown_math::isDisplay(cmark_node *node) {
  if (!isMath(node)) return false;
  const auto source = QByteArrayView(cmark_node_get_string_content(node));
  return source.startsWith("$$");
}

QString markdown_math::render(cmark_node *node, const QString &color, math::Resources &resources) {
  const auto source = QString::fromUtf8(cmark_node_get_string_content(node));
  const bool display = source.startsWith("$$");
  const int delimiter = display ? 2 : 1;
  return render(source.mid(delimiter, source.size() - delimiter * 2), source, display, color, resources);
}

QString markdown_math::render(const QString &latex, const QString &source, bool display, const QString &color,
                              math::Resources &resources) {
  const auto resource = math::render(latex, display, QColor(color));
  if (!resource) return source.toHtmlEscaped();
  resources.emplace_back(resource);
  const auto image = QStringLiteral("<img src=\"%1\" alt=\"%2\" align=\"middle\"/>")
                         .arg(resource->url, source.toHtmlEscaped());
  return display ? QStringLiteral("<p align=\"center\">%1</p>").arg(image) : image;
}
