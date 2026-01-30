#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <format>
#include <vector>
#include "snippet-db.hpp"

SnippetDatabase::SnippetDatabase(std::filesystem::path path) : m_path(path) {
  if (!std::filesystem::is_regular_file(m_path)) {
    if (const auto result = setSnippets({}); !result) {
      qCritical() << "Unable to create default snippet file at" << m_path.c_str() << result.error().c_str();
    }
  }
}

std::expected<std::vector<SnippetDatabase::SerializedSnippet>, std::string> SnippetDatabase::loadSnippets() {
  std::vector<SnippetDatabase::SerializedSnippet> snippets;

  if (const auto error = glz::read_file_json(snippets, m_path.c_str(), m_buf)) {
    return std::unexpected(glz::format_error(error));
  }

  return snippets;
}

std::expected<void, std::string> SnippetDatabase::addSnippet(SnippetDatabase::SerializedSnippet snippet) {
  auto existing = loadSnippets();

  if (!existing) {
    return std::unexpected(std::format("Failed to load existing snippets: {}", existing.error()));
  }

  auto snippets = std::move(existing).value();
  snippets.emplace_back(snippet);
  return setSnippets(snippets);
}

std::expected<void, std::string>
SnippetDatabase::setSnippets(std::span<SnippetDatabase::SerializedSnippet> snippets) {
  if (const auto error = glz::write_file_json(snippets, m_path.c_str(), m_buf)) {
    return std::unexpected(std::format("Failed to save snippets on disk: {}", glz::format_error(error)));
  }

  return {};
}

template <> struct glz::meta<SnippetDatabase::FileSnippet> {
  using mimic = std::string;
  static constexpr auto value = &SnippetDatabase::FileSnippet::file;
};

template <> struct glz::meta<SnippetDatabase::TextSnippet> {
  using mimic = std::string;
  static constexpr auto value = &SnippetDatabase::TextSnippet::text;
};
