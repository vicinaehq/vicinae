#pragma once
#include <algorithm>
#include <cctype>
#include <QCoreApplication>
#include <QDebug>
#include <cstdint>
#include <format>
#include <variant>
#include <filesystem>
#include <string>
#include <expected>

namespace snippet {

struct FileSnippet {
  std::string file;
};
struct TextSnippet {
  std::string text;
};

struct Expansion {
  Q_DECLARE_TR_FUNCTIONS(Expansion)

public:
  std::string keyword;
  // Older files keep the apps here. Moved to the snippet on load, so this is never set afterwards.
  std::optional<std::vector<std::string>> apps;
  bool word = true;

  static std::optional<std::string> validateKeyword(const std::string &keyword) {
    if (keyword.empty()) return tr("Keyword cannot be empty").toStdString();
    if (keyword.size() > 32) return tr("Keyword exceeds maximum length of %1").arg(32).toStdString();
    if (!std::ranges::all_of(keyword, [](char c) {
          auto uc = static_cast<unsigned char>(c);
          return uc <= 127 && std::isprint(uc) && !std::isspace(uc);
        }))
      return tr("Keyword must only contain printable ASCII characters (no spaces)").toStdString();
    return std::nullopt;
  }
};

using SnippetData = std::variant<FileSnippet, TextSnippet>;

/**
 * What two spoken phrases are compared on: speech recognition is inconsistent about case, accents,
 * punctuation and word boundaries ("E-mail." and "email"), so only folded letters and numbers are kept.
 */
std::string normalizeVoicePhrase(std::string_view text);

struct SnippetPayload {
  std::string name;
  SnippetData data;
  std::optional<Expansion> expansion;
  std::vector<std::string> apps;
  std::optional<std::string> voicePhrase;
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
  // Where the keyword and the voice phrase apply. Empty means everywhere.
  std::vector<std::string> apps;
  std::optional<std::string> voicePhrase;
};

} // namespace snippet

// basic snippet CRUD
class SnippetDatabase {
  Q_DECLARE_TR_FUNCTIONS(SnippetDatabase)

public:
  static constexpr size_t MAX_SNIPPETS = 10000;
  static constexpr size_t MAX_KEYWORD_LENGTH = 32;

  SnippetDatabase(const std::filesystem::path &path);

  std::expected<snippet::SerializedSnippet, std::string> addSnippet(const snippet::SnippetPayload &snippet);
  std::expected<void, std::string> setSnippets(std::span<snippet::SerializedSnippet> snippets);
  std::expected<void, std::string> updateSnippet(std::string_view id, const snippet::SnippetPayload &payload);
  std::expected<snippet::SerializedSnippet, std::string> removeSnippet(std::string_view id);

  std::vector<snippet::SerializedSnippet> snippets() const;

  snippet::SerializedSnippet *findByKeyword(std::string_view keyword);
  snippet::SerializedSnippet *findByVoicePhrase(std::string_view phrase);

  std::vector<std::string> voicePhrases() const;
  snippet::SerializedSnippet *findById(std::string_view id);

protected:
  std::expected<std::vector<snippet::SerializedSnippet>, std::string> loadSnippets();

private:
  std::string m_buf;
  std::filesystem::path m_path;
  std::vector<snippet::SerializedSnippet> m_snippets;
};
