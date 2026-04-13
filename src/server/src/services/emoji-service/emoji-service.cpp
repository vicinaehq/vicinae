#include "emoji-service.hpp"
#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include "fuzzy/fzf.hpp"
#include "omni-database.hpp"
#include <qlogging.h>
#include "utils/utils.hpp"

namespace fs = std::filesystem;

namespace {

std::optional<emoji::SkinTone> toneFromId(std::string_view id) {
  for (auto const &info : emoji::skinTones()) {
    if (info.id == id) return info.tone;
  }
  return std::nullopt;
}

EmojiWithMetadata toMetadata(const SerializedEmojiMetadata &entry, const EmojiData *data) {
  EmojiWithMetadata meta;
  meta.data = data;
  meta.visitCount = entry.visitCount;
  if (entry.pinnedAt) { meta.pinnedAt = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(*entry.pinnedAt)); }
  if (entry.skinTone) { meta.tone = toneFromId(*entry.skinTone); }
  return meta;
}

} // namespace

EmojiService::EmojiService(const std::filesystem::path &path, OmniDatabase *legacyDb) : m_path(path) {
  if (!fs::is_regular_file(m_path)) {
    fs::create_directories(m_path.parent_path());
    save();
  }

  load();

  if (legacyDb && m_entries.empty()) { migrateFromDatabase(*legacyDb); }
}

bool EmojiService::load() {
  if (const auto error = glz::read_file_json(m_entries, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to read emoji metadata:" << glz::format_error(error).c_str();
    m_entries.clear();
    return false;
  }
  return true;
}

bool EmojiService::save() {
  if (const auto error = glz::write_file_json(m_entries, m_path.c_str(), m_buf)) {
    qCritical() << "Failed to save emoji metadata:" << glz::format_error(error).c_str();
    return false;
  }
  return true;
}

void EmojiService::migrateFromDatabase(OmniDatabase &db) {
  auto check = db.db().prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='visited_emoji'");

  if (!check.step()) return;

  auto stmt = db.db().prepare(R"(
    SELECT emoji, visit_count, pinned_at, last_visited_at
    FROM visited_emoji
  )");

  std::vector<SerializedEmojiMetadata> migrated;

  while (stmt.step()) {
    SerializedEmojiMetadata entry;
    entry.emoji = stmt.columnText(0);
    entry.visitCount = static_cast<std::uint32_t>(stmt.columnInt(1));
    if (!stmt.isNull(2)) { entry.pinnedAt = stmt.columnUInt64(2); }
    if (!stmt.isNull(3)) { entry.lastVisitedAt = stmt.columnUInt64(3); }
    migrated.emplace_back(std::move(entry));
  }

  if (migrated.empty()) return;

  m_entries = std::move(migrated);

  if (!save()) return;

  qInfo() << "Migrated" << m_entries.size() << "emoji metadata entries from database to JSON file";
}

SerializedEmojiMetadata *EmojiService::findEntry(std::string_view emoji) {
  auto it = std::ranges::find(m_entries, emoji, &SerializedEmojiMetadata::emoji);
  return it != m_entries.end() ? &*it : nullptr;
}

SerializedEmojiMetadata &EmojiService::entryFor(std::string_view emoji) {
  auto it = std::ranges::find(m_entries, emoji, &SerializedEmojiMetadata::emoji);
  if (it != m_entries.end()) return *it;
  return m_entries.emplace_back(SerializedEmojiMetadata{.emoji = std::string(emoji)});
}

std::span<Scored<const EmojiData *>> EmojiService::search(std::string_view query) const {
  static std::vector<Scored<const EmojiData *>> searchResults;
  auto withScore = [&](const EmojiData &data) -> Scored<const EmojiData *> {
    using WS = fzf::WeightedString;

    std::initializer_list<WS> fields = {WS{data.name, 1.0f}, WS{data.group, 0.5f}};
    auto kws = data.keywords | std::views::transform([](auto &&s) { return WS{s, 1.0f}; });
    int const score = fzf::threadLocalMatcher().fuzzy_match_v2_score_query(fields, kws, query);

    return {&data, score};
  };

  auto filtered = StaticEmojiDatabase::orderedList() | std::views::transform(withScore) |
                  std::views::filter([](auto &&s) { return s.score > 0; });
  searchResults.clear();
  std::ranges::copy(filtered, std::back_inserter(searchResults));
  std::ranges::stable_sort(searchResults, std::greater{});

  return searchResults;
}

bool EmojiService::registerVisit(std::string_view emoji) {
  auto &entry = entryFor(emoji);
  entry.visitCount += 1;
  entry.lastVisitedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  if (!save()) return false;

  emit visited(emoji);

  return true;
}

std::vector<std::pair<std::string_view, std::vector<const EmojiData *>>> EmojiService::grouped() {
  std::unordered_map<std::string_view, std::vector<const EmojiData *>> map;

  for (const auto &emoji : StaticEmojiDatabase::orderedList()) {
    map[emoji.group].emplace_back(&emoji);
  }

  std::vector<std::pair<std::string_view, std::vector<const EmojiData *>>> grouped;
  auto &groups = StaticEmojiDatabase::groups();

  grouped.reserve(groups.size());

  for (auto group : groups) {
    grouped.emplace_back(group, std::move(map[group]));
  }

  return grouped;
}

std::vector<EmojiWithMetadata> EmojiService::getVisited() const {
  std::vector<const SerializedEmojiMetadata *> sorted;
  sorted.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    sorted.emplace_back(&entry);
  }

  std::ranges::sort(sorted, [](const SerializedEmojiMetadata *a, const SerializedEmojiMetadata *b) {
    auto const ap = a->pinnedAt.value_or(0), bp = b->pinnedAt.value_or(0);
    if (ap != bp) return ap > bp;
    if (a->visitCount != b->visitCount) return a->visitCount > b->visitCount;
    return a->lastVisitedAt.value_or(0) > b->lastVisitedAt.value_or(0);
  });

  std::vector<EmojiWithMetadata> results;
  results.reserve(sorted.size());
  const auto &mapping = StaticEmojiDatabase::mapping();

  for (const auto *entry : sorted) {
    auto it = mapping.find(entry->emoji);

    if (it == mapping.end()) {
      qWarning() << "Emoji is not in the mapping" << QString::fromStdString(entry->emoji);
      continue;
    }

    results.emplace_back(toMetadata(*entry, it->second));
  }

  return results;
}

std::vector<EmojiWithMetadata> EmojiService::mapMetadata(const std::vector<const EmojiData *> &items) {
  if (items.empty()) return {};

  std::unordered_map<std::string_view, const SerializedEmojiMetadata *> byEmoji;
  byEmoji.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    byEmoji.emplace(entry.emoji, &entry);
  }

  std::vector<EmojiWithMetadata> metadatas;
  metadatas.reserve(items.size());

  for (const auto *item : items) {
    if (auto it = byEmoji.find(item->emoji); it != byEmoji.end()) {
      metadatas.emplace_back(toMetadata(*it->second, item));
    } else {
      metadatas.emplace_back(EmojiWithMetadata{.data = item});
    }
  }

  return metadatas;
}

EmojiWithMetadata EmojiService::mapMetadata(std::string_view emoji) {
  auto it = StaticEmojiDatabase::mapping().find(emoji);

  if (it == StaticEmojiDatabase::mapping().end()) { return {}; }

  if (const auto *entry = findEntry(emoji)) { return toMetadata(*entry, it->second); }

  return {.data = it->second};
}

bool EmojiService::resetRanking(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->visitCount = 0;
    entry->lastVisitedAt = std::nullopt;
    if (!save()) return false;
  }

  emit rankingReset(emoji);

  return true;
}

bool EmojiService::unpin(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->pinnedAt = std::nullopt;
    if (!save()) return false;
  }

  emit unpinned(emoji);

  return true;
}

bool EmojiService::pin(std::string_view emoji) {
  auto &entry = entryFor(emoji);
  entry.pinnedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  if (!save()) return false;

  emit pinned(emoji);

  return true;
}

bool EmojiService::setSkinTone(std::string_view emoji, emoji::SkinTone tone) {
  auto &entry = entryFor(emoji);
  entry.skinTone = std::string(emoji::skinToneInfo(tone).id);

  if (!save()) return false;

  emit skintoneChanged(emoji);

  return true;
}

bool EmojiService::resetSkinTone(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->skinTone = std::nullopt;
    if (!save()) return false;
  }

  emit skintoneChanged(emoji);

  return true;
}
