#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <format>
#include <vector>
#include "snippet-db.hpp"
#include "utils.hpp"

SnippetDatabase::SnippetDatabase(std::filesystem::path path) : m_path(path) {
  if (!std::filesystem::is_regular_file(m_path)) {
    if (const auto result = setSnippets({}); !result) {
      qCritical() << "Unable to create default snippet file at" << m_path.c_str() << result.error().c_str();
    }
  }

  m_snippets = loadSnippets().value_or({});
}

std::expected<std::vector<SnippetDatabase::SerializedSnippet>, std::string> SnippetDatabase::loadSnippets() {
  std::vector<SnippetDatabase::SerializedSnippet> snippets;

  if (const auto error = glz::read_file_json(snippets, m_path.c_str(), m_buf)) {
    return std::unexpected(glz::format_error(error));
  }

  return snippets;
}

std::vector<SnippetDatabase::SerializedSnippet> SnippetDatabase::snippets() const { return m_snippets; }

std::expected<void, std::string> SnippetDatabase::updateSnippet(std::string_view id, SnippetPayload payload) {
  if (const auto snippet = findById(id)) {
    snippet->name = payload.name;
    snippet->trigger = payload.trigger;
    snippet->data = payload.data;
    snippet->word = payload.word;

    return setSnippets(m_snippets);
  }

  return std::unexpected("No snippet with that ID");
}

std::expected<void, std::string> SnippetDatabase::removeSnippet(std::string_view id) {
  if (auto it = std::ranges::find_if(m_snippets, [&](auto &&item) { return item.id == id; });
      it != m_snippets.end()) {
    m_snippets.erase(it);
    return setSnippets(m_snippets);
  }

  return std::unexpected("No such snippet");
}

SnippetDatabase::SerializedSnippet *SnippetDatabase::findById(std::string_view id) {
  if (auto it = std::ranges::find_if(m_snippets, [&](auto &&item) { return item.id == id; });
      it != m_snippets.end()) {
    return &*it;
  }
  return nullptr;
}

std::expected<SnippetDatabase::SerializedSnippet, std::string>
SnippetDatabase::addSnippet(SnippetDatabase::SnippetPayload snippet) {
  if (m_snippets.size() >= MAX_SNIPPETS) {
    return std::unexpected(std::format("Snippet limit reached ({})", MAX_SNIPPETS));
  }

  SerializedSnippet serialized = {
      .id = generatePrefixedId("snp"),
      .name = snippet.name,
      .trigger = snippet.trigger,
      .word = snippet.word,
      .data = snippet.data,
  };

  m_snippets.emplace_back(serialized);

  return setSnippets(m_snippets).transform([&]() { return serialized; });
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
