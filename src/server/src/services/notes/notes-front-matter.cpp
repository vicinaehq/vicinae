#include <algorithm>
#include <cctype>
#include <optional>
#include "notes-front-matter.hpp"

namespace Notes {
namespace {

constexpr std::string_view DELIMITER = "---";
constexpr std::string_view TAGS_KEY = "tags:";
constexpr std::string_view WHITESPACE = " \t\r\n";

struct FrontMatter {
  // Up to the `tags` key, the lines of that key, then the rest of the block and its closing delimiter.
  std::string_view before;
  std::string_view tags;
  std::string_view after;
  std::string_view body;
};

std::string_view trimmed(std::string_view text) {
  const auto first = text.find_first_not_of(WHITESPACE);
  if (first == std::string_view::npos) return {};
  return text.substr(first, text.find_last_not_of(WHITESPACE) - first + 1);
}

std::optional<FrontMatter> parse(std::string_view markdown) {
  const auto lineEnd = [markdown](std::size_t start) {
    const auto newline = markdown.find('\n', start);
    return newline == std::string_view::npos ? markdown.size() : newline + 1;
  };
  const auto isDelimiter = [](std::string_view line) {
    return line.starts_with(DELIMITER) && trimmed(line) == DELIMITER;
  };

  const auto opening = lineEnd(0);
  if (!isDelimiter(markdown.substr(0, opening))) return std::nullopt;

  std::optional<std::size_t> tagsStart;
  std::optional<std::size_t> tagsEnd;
  bool hasKeys = false;
  for (auto start = opening; start < markdown.size();) {
    const auto end = lineEnd(start);
    const auto line = markdown.substr(start, end - start);
    if (isDelimiter(line)) {
      if (!hasKeys) return std::nullopt;
      const auto from = tagsStart.value_or(start);
      const auto to = tagsEnd.value_or(start);
      return FrontMatter{.before = markdown.substr(0, from),
                         .tags = markdown.substr(from, to - from),
                         .after = markdown.substr(to, end - to),
                         .body = markdown.substr(end)};
    }
    const bool continuation = line.starts_with(' ') || line.starts_with('\t') || line.starts_with('-');
    if (tagsStart && !tagsEnd && !continuation) tagsEnd = start;
    if (!continuation && !trimmed(line).empty() && !line.starts_with('#')) {
      // A thematic break followed by prose is not front matter.
      if (!line.contains(':')) return std::nullopt;
      hasKeys = true;
      if (!tagsStart && line.starts_with(TAGS_KEY)) tagsStart = start;
    }
    start = end;
  }
  return std::nullopt;
}

} // namespace

bool isValidTag(std::string_view tag) {
  const auto allowed = [](unsigned char c) {
    return std::isalnum(c) || c == '-' || c == '_' || c == '/' || c >= 0x80;
  };
  return !tag.empty() && !tag.starts_with('-') && std::ranges::all_of(tag, allowed);
}

std::string_view body(std::string_view markdown) {
  const auto matter = parse(markdown);
  return matter ? matter->body : markdown;
}

std::vector<std::string> tags(std::string_view markdown) {
  const auto matter = parse(markdown);
  if (!matter || matter->tags.empty()) return {};

  std::vector<std::string> result;
  result.reserve(4);
  auto value = matter->tags.substr(TAGS_KEY.size());
  while (!value.empty()) {
    const auto end = std::min(value.find_first_of(",[]\n"), value.size());
    auto tag = trimmed(value.substr(0, end));
    value.remove_prefix(std::min(end + 1, value.size()));
    if (tag.starts_with("- ")) tag = trimmed(tag.substr(2));
    if (tag.size() >= 2 && (tag.front() == '"' || tag.front() == '\'') && tag.back() == tag.front())
      tag = tag.substr(1, tag.size() - 2);
    if (tag.starts_with('#')) tag.remove_prefix(1);
    if (isValidTag(tag) && std::ranges::find(result, tag) == result.end()) result.emplace_back(tag);
  }
  return result;
}

std::string withTags(std::string_view markdown, std::span<const std::string> tags) {
  const std::string_view eol = markdown.contains("\r\n") ? "\r\n" : "\n";
  std::string line;
  if (!tags.empty()) {
    line += "tags: [";
    for (const auto &tag : tags) {
      if (&tag != &tags.front()) line += ", ";
      line += tag;
    }
    line += "]";
    line += eol;
  }

  const auto matter = parse(markdown);
  std::string result;
  result.reserve(markdown.size() + line.size() + 2 * (DELIMITER.size() + eol.size()));
  if (!matter) {
    if (line.empty()) return std::string(markdown);
    result.append(DELIMITER).append(eol).append(line).append(DELIMITER).append(eol);
    return result.append(markdown);
  }
  // Removing the last tag of a block that holds nothing else removes the block.
  if (line.empty() && trimmed(matter->before) == DELIMITER && trimmed(matter->after) == DELIMITER)
    return std::string(matter->body);
  return result.append(matter->before).append(line).append(matter->after).append(matter->body);
}

} // namespace Notes
