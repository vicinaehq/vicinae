#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include <format>
#include <ranges>
#include <vector>
#include "snippet-db.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

using namespace snippet;

std::string snippet::normalizeVoicePhrase(std::string_view text) {
  const auto decomposed = QString::fromUtf8(text.data(), static_cast<qsizetype>(text.size()))
                              .normalized(QString::NormalizationForm_KD)
                              .toCaseFolded();
  QString normalized;
  normalized.reserve(decomposed.size());

  for (const auto codepoint : decomposed.toUcs4()) {
    const auto c = static_cast<char32_t>(codepoint);
    if (QChar::isLetterOrNumber(c)) normalized += QChar::fromUcs4(c);
  }

  return normalized.toStdString();
}

SnippetDatabase::SnippetDatabase(const std::filesystem::path &path) : m_path(path) {
  if (!fs::is_regular_file(m_path)) {
    fs::create_directories(path.parent_path());
    if (const auto result = setSnippets({}); !result) {
      qCritical() << "Unable to create default snippet file at" << m_path.string() << result.error().c_str();
    }
  }

  auto loaded = loadSnippets();
  if (!loaded) {
    // the next save would replace the file with an empty list
    auto backup = m_path;
    backup += ".invalid";
    std::error_code ec;
    fs::copy_file(m_path, backup, fs::copy_options::overwrite_existing, ec);
    qCritical() << "Could not read snippets, keeping a copy at" << backup.string() << ":"
                << loaded.error().c_str();
  }
  m_snippets = std::move(loaded).value_or(std::vector<SerializedSnippet>{});

  for (auto &snippet : m_snippets) {
    if (!snippet.expansion || !snippet.expansion->apps) continue;
    snippet.apps = std::move(*snippet.expansion->apps);
    snippet.expansion->apps.reset();
  }
}

std::expected<std::vector<SerializedSnippet>, std::string> SnippetDatabase::loadSnippets() {
  std::vector<SerializedSnippet> snippets;

  // Fields from a newer version are ignored rather than failing, which would make every snippet look gone.
  if (const auto error =
          glz::read_file_json<glz::opts{.error_on_unknown_keys = false}>(snippets, m_path.string(), m_buf)) {
    return std::unexpected(glz::format_error(error));
  }

  return snippets;
}

std::vector<SerializedSnippet> SnippetDatabase::snippets() const { return m_snippets; }

std::expected<void, std::string> SnippetDatabase::updateSnippet(std::string_view id,
                                                                const SnippetPayload &payload) {
  if (const auto e = payload.expansion) {
    if (const auto existing = findByKeyword(e->keyword); existing && existing->id != id) {
      return std::unexpected(
          tr("keyword already assigned to \"%1\"").arg(existing->name.c_str()).toStdString());
    }
  }

  if (payload.voicePhrase) {
    if (const auto existing = findByVoicePhrase(*payload.voicePhrase); existing && existing->id != id) {
      return std::unexpected(
          tr("voice phrase already assigned to \"%1\"").arg(existing->name.c_str()).toStdString());
    }
  }

  if (const auto snippet = findById(id)) {
    snippet->name = payload.name;
    snippet->expansion = payload.expansion;
    snippet->apps = payload.apps;
    snippet->voicePhrase = payload.voicePhrase;
    snippet->data = payload.data;
    snippet->updatedAt = QDateTime::currentSecsSinceEpoch();

    return setSnippets(m_snippets);
  }

  return std::unexpected(tr("No snippet with that ID").toStdString());
}

std::expected<SerializedSnippet, std::string> SnippetDatabase::removeSnippet(std::string_view id) {
  if (auto it = std::ranges::find_if(m_snippets, [&](auto &&item) { return item.id == id; });
      it != m_snippets.end()) {
    auto snip = *it;

    m_snippets.erase(it);

    if (const auto result = setSnippets(m_snippets); !result) { return std::unexpected(result.error()); }

    return snip;
  }

  return std::unexpected(tr("No such snippet").toStdString());
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

SerializedSnippet *SnippetDatabase::findByVoicePhrase(std::string_view phrase) {
  const auto normalized = normalizeVoicePhrase(phrase);
  if (normalized.empty()) return nullptr;

  if (auto it = std::ranges::find_if(m_snippets,
                                     [&](auto &&item) {
                                       return item.voicePhrase &&
                                              normalizeVoicePhrase(*item.voicePhrase) == normalized;
                                     });
      it != m_snippets.end()) {
    return &*it;
  }
  return nullptr;
}

std::vector<std::string> SnippetDatabase::voicePhrases() const {
  return m_snippets |
         std::views::filter([](const SerializedSnippet &s) { return s.voicePhrase.has_value(); }) |
         std::views::transform([](const SerializedSnippet &s) { return *s.voicePhrase; }) |
         std::ranges::to<std::vector>();
}

std::expected<SerializedSnippet, std::string> SnippetDatabase::addSnippet(const SnippetPayload &snippet) {
  if (m_snippets.size() >= MAX_SNIPPETS) {
    return std::unexpected(tr("Snippet limit reached (%1)").arg(MAX_SNIPPETS).toStdString());
  }

  if (const auto e = snippet.expansion) {
    if (const auto existing = findByKeyword(e->keyword)) {
      return std::unexpected(
          tr("keyword already assigned to \"%1\"").arg(existing->name.c_str()).toStdString());
    }
  }

  if (snippet.voicePhrase) {
    if (const auto existing = findByVoicePhrase(*snippet.voicePhrase)) {
      return std::unexpected(
          tr("voice phrase already assigned to \"%1\"").arg(existing->name.c_str()).toStdString());
    }
  }

  SerializedSnippet serialized = {
      .id = generatePrefixedId("snp"),
      .name = snippet.name,
      .data = snippet.data,
      .createdAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch()),
      .expansion = snippet.expansion,
      .apps = snippet.apps,
      .voicePhrase = snippet.voicePhrase,
  };

  m_snippets.emplace_back(serialized);

  return setSnippets(m_snippets).transform([&]() { return serialized; });
}

std::expected<void, std::string> SnippetDatabase::setSnippets(std::span<SerializedSnippet> snippets) {
  if (const auto error = glz::write_file_json(snippets, m_path.string(), m_buf)) {
    return std::unexpected(
        tr("Failed to save snippets on disk: %1").arg(glz::format_error(error).c_str()).toStdString());
  }

  return {};
}
