#pragma once
#include <QDebug>
#include <variant>
#include <filesystem>
#include <string>
#include <expected>

// basic snippet CRUD
class SnippetDatabase {
public:
  struct FileSnippet {
    std::string file;
  };
  struct TextSnippet {
    std::string text;
  };

  using SnippetData = std::variant<FileSnippet, TextSnippet>;

  struct SnippetPayload {
    std::string name;
    std::optional<std::string> trigger;
    bool word = false;
    SnippetData data;
  };

  struct SerializedSnippet {
    std::string id;
    std::string name;
    std::optional<std::string> trigger;
    bool word = false;
    SnippetData data;
  };

  static constexpr size_t MAX_SNIPPETS = 10000;

  SnippetDatabase(std::filesystem::path path);

  std::expected<SerializedSnippet, std::string> addSnippet(SnippetPayload snippet);
  std::expected<void, std::string> setSnippets(std::span<SerializedSnippet> snippets);
  std::expected<void, std::string> updateSnippet(std::string_view id, SnippetPayload payload);
  std::expected<void, std::string> removeSnippet(std::string_view id);

  std::vector<SerializedSnippet> snippets() const;

protected:
  std::expected<std::vector<SerializedSnippet>, std::string> loadSnippets();
  SerializedSnippet *findById(std::string_view id);

private:
  std::string m_buf;
  std::filesystem::path m_path;
  std::vector<SerializedSnippet> m_snippets;
};
