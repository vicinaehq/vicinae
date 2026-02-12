#pragma once
#include <QDebug>
#include <cstdint>
#include <variant>
#include <filesystem>
#include <string>
#include <expected>

struct FileSnippet {
  std::string file;
};
struct TextSnippet {
  std::string text;
};

struct Expansion {
  std::string keyword;
  std::vector<std::string> apps;
  bool word = true;
};

using SnippetData = std::variant<FileSnippet, TextSnippet>;

struct SnippetPayload {
  std::string name;
  SnippetData data;
  std::optional<Expansion> expansion;
};

struct SerializedSnippet {
  std::string id;
  std::string name;
  SnippetData data;
  std::uint64_t createdAt;
  std::optional<std::uint64_t> updatedAt;

  // snippet may not necessarily define an expansion trigger, just be used
  // for paste.
  std::optional<Expansion> expansion;
};

// basic snippet CRUD
class SnippetDatabase {
public:
  static constexpr size_t MAX_SNIPPETS = 10000;

  SnippetDatabase(std::filesystem::path path);

  std::expected<SerializedSnippet, std::string> addSnippet(SnippetPayload snippet);
  std::expected<void, std::string> setSnippets(std::span<SerializedSnippet> snippets);
  std::expected<void, std::string> updateSnippet(std::string_view id, SnippetPayload payload);
  std::expected<SerializedSnippet, std::string> removeSnippet(std::string_view id);

  std::vector<SerializedSnippet> snippets() const;

  SerializedSnippet *findByKeyword(std::string_view keyword);
  SerializedSnippet *findById(std::string_view id);

protected:
  std::expected<std::vector<SerializedSnippet>, std::string> loadSnippets();

private:
  std::string m_buf;
  std::filesystem::path m_path;
  std::vector<SerializedSnippet> m_snippets;
};
