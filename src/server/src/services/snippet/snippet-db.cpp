#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <format>
#include <vector>
#include "snippet-db.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

using namespace snippet;

SnippetDatabase::SnippetDatabase(std::filesystem::path path) : m_path(path) {
  if (!fs::is_regular_file(m_path)) {
    fs::create_directories(path.parent_path());
    if (const auto result = setSnippets({}); !result) {
      qCritical() << "Unable to create default snippet file at" << m_path.c_str() << result.error().c_str();
    }
  }

  m_snippets = loadSnippets().value_or({});
}

std::expected<std::vector<SerializedSnippet>, std::string> SnippetDatabase::loadSnippets() {
  std::vector<SerializedSnippet> snippets;

  if (const auto error = glz::read_file_json(snippets, m_path.c_str(), m_buf)) {
    return std::unexpected(glz::format_error(error));
  }

  return snippets;
}

std::vector<SerializedSnippet> SnippetDatabase::snippets() const { return m_snippets; }

std::expected<void, std::string> SnippetDatabase::updateSnippet(std::string_view id, SnippetPayload payload) {
  if (const auto e = payload.expansion) {
    if (const auto existing = findByKeyword(e->keyword); existing && existing->id != id) {
      return std::unexpected(std::format("keyword already assigned to \"{}\"", existing->name));
    }
  }

  if (const auto snippet = findById(id)) {
    snippet->name = payload.name;
    snippet->expansion = payload.expansion;
    snippet->data = payload.data;
    snippet->updatedAt = QDateTime::currentSecsSinceEpoch();

    return setSnippets(m_snippets);
  }

  return std::unexpected("No snippet with that ID");
}

std::expected<SerializedSnippet, std::string>
SnippetDatabase::removeSnippet(std::string_view id) {
  if (auto it = std::ranges::find_if(m_snippets, [&](auto &&item) { return item.id == id; });
      it != m_snippets.end()) {
    auto snip = *it;

    m_snippets.erase(it);

    if (const auto result = setSnippets(m_snippets); !result) { return std::unexpected(result.error()); }

    return snip;
  }

  return std::unexpected("No such snippet");
}

SerializedSnippet *SnippetDatabase::findById(std::string_view id) {
  if (auto it = std::ranges::find_if(m_snippets, [&](auto &&item) { return item.id == id; });
      it != m_snippets.end()) {
    return &*it;
  }
  return nullptr;
}

SerializedSnippet *SnippetDatabase::findByKeyword(std::string_view keyword) {
  if (auto it = std::ranges::find_if(
          m_snippets, [&](auto &&item) { return item.expansion && item.expansion->keyword == keyword; });
      it != m_snippets.end()) {
    return &*it;
  }
  return nullptr;
}

std::expected<SerializedSnippet, std::string>
SnippetDatabase::addSnippet(SnippetPayload snippet) {
  if (m_snippets.size() >= MAX_SNIPPETS) {
    return std::unexpected(std::format("Snippet limit reached ({})", MAX_SNIPPETS));
  }

  if (const auto e = snippet.expansion) {
    if (const auto existing = findByKeyword(e->keyword)) {
      return std::unexpected(std::format("keyword already assigned to \"{}\"", existing->name));
    }
  }

  SerializedSnippet serialized = {
      .id = generatePrefixedId("snp"),
      .name = snippet.name,
      .data = snippet.data,
      .createdAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch()),
      .expansion = snippet.expansion,
  };

  m_snippets.emplace_back(serialized);

  return setSnippets(m_snippets).transform([&]() { return serialized; });
}

std::expected<void, std::string>
SnippetDatabase::setSnippets(std::span<SerializedSnippet> snippets) {
  if (const auto error = glz::write_file_json(snippets, m_path.c_str(), m_buf)) {
    return std::unexpected(std::format("Failed to save snippets on disk: {}", glz::format_error(error)));
  }

  return {};
}
