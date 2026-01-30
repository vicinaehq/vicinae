#pragma once
#include <cstdint>
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

  struct SerializedSnippet {
    std::string trigger;
    bool word = false;
    SnippetData data;
    std::uint64_t createdAt;
    std::uint64_t updatedAt;
  };

  SnippetDatabase(std::filesystem::path path);
  std::expected<std::vector<SerializedSnippet>, std::string> loadSnippets();
  std::expected<void, std::string> addSnippet(SerializedSnippet snippet);
  std::expected<void, std::string> setSnippets(std::span<SerializedSnippet> snippets);

private:
  std::string m_buf;
  std::filesystem::path m_path;
};
