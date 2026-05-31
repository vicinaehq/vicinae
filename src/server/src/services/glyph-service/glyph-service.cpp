#include "glyph-service.hpp"
#include <algorithm>
#include <bits/ranges_algo.h>
#include <filesystem>
#include <glaze/core/reflect.hpp>
#include <glaze/json/read.hpp>
#include <glaze/json/write.hpp>
#include "fuzzy/fzf.hpp"
#include "glyph/glyph.hpp"
#include "omni-database.hpp"
#include <qfuture.h>
#include <qfuturesynchronizer.h>
#include <qlogging.h>
#include <qtconcurrentrun.h>
#include <qthreadpool.h>
#include <ranges>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

namespace {

std::optional<emoji::SkinTone> toneFromId(std::string_view id) {
  for (auto const &info : emoji::skinTones()) {
    if (info.id == id) return info.tone;
  }
  return std::nullopt;
}

GlyphMetadata toMetadata(const SerializedEmojiMetadata &entry, const glyph::Item *data) {
  GlyphMetadata meta;
  meta.data = data;
  meta.visitCount = entry.visitCount;
  if (entry.pinnedAt) { meta.pinnedAt = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(*entry.pinnedAt)); }
  if (entry.skinTone) { meta.tone = toneFromId(*entry.skinTone); }
  if (entry.keyword) { meta.keyword = entry.keyword.value(); }
  return meta;
}

} // namespace

GlyphService::GlyphService(const std::filesystem::path &path, OmniDatabase *legacyDb) : m_path(path) {
  if (!fs::is_regular_file(m_path)) {
    fs::create_directories(m_path.parent_path());
    save();
  }

  load();

  if (legacyDb && m_entries.empty()) { migrateFromDatabase(*legacyDb); }
}

bool GlyphService::load() {
  if (const auto error = glz::read_file_json(m_entries, m_path.c_str(), m_buf)) {
    qWarning() << "Failed to read emoji metadata:" << glz::format_error(error).c_str();
    m_entries.clear();
    m_indexDirty = true;
    return false;
  }
  m_indexDirty = true;
  return true;
}

bool GlyphService::save() {
  if (const auto error = glz::write_file_json(m_entries, m_path.c_str(), m_buf)) {
    qCritical() << "Failed to save emoji metadata:" << glz::format_error(error).c_str();
    return false;
  }
  return true;
}

void GlyphService::migrateFromDatabase(OmniDatabase &db) {
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
  m_indexDirty = true;

  if (!save()) return;

  qInfo() << "Migrated" << m_entries.size() << "emoji metadata entries from database to JSON file";
}

SerializedEmojiMetadata const *GlyphService::findEntry(std::string_view emoji) const {
  auto it = std::ranges::find(m_entries, emoji, &SerializedEmojiMetadata::emoji);
  return it != m_entries.end() ? &*it : nullptr;
}

SerializedEmojiMetadata *GlyphService::findEntry(std::string_view emoji) {
  auto it = std::ranges::find(m_entries, emoji, &SerializedEmojiMetadata::emoji);
  return it != m_entries.end() ? &*it : nullptr;
}

SerializedEmojiMetadata &GlyphService::entryFor(std::string_view emoji) {
  auto it = std::ranges::find(m_entries, emoji, &SerializedEmojiMetadata::emoji);
  if (it != m_entries.end()) return *it;
  m_indexDirty = true;
  return m_entries.emplace_back(SerializedEmojiMetadata{.emoji = std::string(emoji)});
}

void GlyphService::ensureIndex() const {
  if (!m_indexDirty) return;
  m_entryByItem.assign(glyph::items().size(), nullptr);
  for (const auto &entry : m_entries)
    if (const auto *item = glyph::lookup(entry.emoji)) m_entryByItem[item - glyph::items().data()] = &entry;
  m_indexDirty = false;
}

std::span<Scored<const glyph::Item *>> GlyphService::search(std::string_view query) const {
  ensureIndex();

  auto withScore = [&](const glyph::Item &data) -> Scored<const glyph::Item *> {
    using WS = fzf::WeightedString;
    const auto *entry = m_entryByItem[&data - glyph::items().data()];

    // user defined keyword, we may or may not have one for this item
    std::string_view kw = entry && entry->keyword ? std::string_view{*entry->keyword} : "";
    std::initializer_list<WS> fields = {WS{kw, 2.0f}, WS{data.name, 1.0f},
                                        WS{glyph::categoryLabel(data.category), 0.5f}};

    auto kws = data.keywords | std::views::transform([](auto &&s) { return WS{s, 0.3f}; });
    int const score = fzf::threadLocalMatcher().fuzzy_match_v2_score_query(fields, kws, query);

    return {&data, score};
  };

  const auto batchCount = std::ceil(glyph::items().size() / 500.0);
  size_t batchSize = 500;

  static std::vector<Scored<const glyph::Item *>> searchResults;

  searchResults.resize(glyph::items().size());

  std::vector<QFuture<size_t>> futures;

  futures.reserve(batchCount);

  for (auto i = 0; i != batchCount; ++i) {
    int startIdx = batchSize * i;
    int endIdx = std::min(startIdx + batchSize, glyph::items().size());

    futures.emplace_back(QtConcurrent::run([withScore, startIdx, endIdx]() {
      size_t zeroCount = 0;
      for (auto i = startIdx; i != endIdx; ++i) {
        auto &glyph = glyph::items()[i];
        searchResults[i] = withScore(glyph);
        if (searchResults[i].score == 0) zeroCount += 1;
      }

      return zeroCount;
    }));
  }

  auto batch = QtFuture::whenAll(futures.begin(), futures.end());
  batch.waitForFinished();

  const size_t zeroCount =
      std::ranges::fold_left(batch.result(), 0, [](auto &&a, auto &&b) { return a + b.result(); });

  std::ranges::stable_sort(searchResults, std::greater{});

  searchResults.resize(searchResults.size() - zeroCount);

  return searchResults;
}

bool GlyphService::registerVisit(std::string_view emoji) {
  auto &entry = entryFor(emoji);
  entry.visitCount += 1;
  entry.lastVisitedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  if (!save()) return false;

  emit visited(emoji);

  return true;
}

std::vector<GlyphMetadata> GlyphService::getVisited() const {
  auto sorted = m_entries | std::views::filter([](auto &&e) { return e.visitCount > 0; }) |
                std::views::transform([](auto &&e) { return &e; }) | std::ranges::to<std::vector>();

  std::ranges::sort(sorted, [](const SerializedEmojiMetadata *a, const SerializedEmojiMetadata *b) {
    auto const ap = a->pinnedAt.value_or(0), bp = b->pinnedAt.value_or(0);
    if (ap != bp) return ap > bp;
    if (a->visitCount != b->visitCount) return a->visitCount > b->visitCount;
    return a->lastVisitedAt.value_or(0) > b->lastVisitedAt.value_or(0);
  });

  std::vector<GlyphMetadata> results;
  results.reserve(sorted.size());

  for (const auto *entry : sorted) {
    if (const auto *item = glyph::lookup(entry->emoji)) { results.emplace_back(toMetadata(*entry, item)); }
  }

  return results;
}

std::vector<GlyphMetadata> GlyphService::mapMetadata(const std::vector<const glyph::Item *> &items) {
  if (items.empty()) return {};

  std::unordered_map<std::string_view, const SerializedEmojiMetadata *> byEmoji;
  byEmoji.reserve(m_entries.size());
  for (const auto &entry : m_entries) {
    byEmoji.emplace(entry.emoji, &entry);
  }

  std::vector<GlyphMetadata> metadatas;
  metadatas.reserve(items.size());

  for (const auto *item : items) {
    if (auto it = byEmoji.find(item->character); it != byEmoji.end()) {
      metadatas.emplace_back(toMetadata(*it->second, item));
    } else {
      metadatas.emplace_back(GlyphMetadata{.data = item});
    }
  }

  return metadatas;
}

GlyphMetadata GlyphService::mapMetadata(std::string_view emoji) {
  const auto *item = glyph::lookup(emoji);

  if (!item) { return {}; }

  if (const auto *entry = findEntry(emoji)) { return toMetadata(*entry, item); }

  return {.data = item};
}

bool GlyphService::resetRanking(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->visitCount = 0;
    entry->lastVisitedAt = std::nullopt;
    if (!save()) return false;
  }

  emit rankingReset(emoji);

  return true;
}

bool GlyphService::unpin(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->pinnedAt = std::nullopt;
    if (!save()) return false;
  }

  emit unpinned(emoji);

  return true;
}

bool GlyphService::pin(std::string_view emoji) {
  auto &entry = entryFor(emoji);
  entry.pinnedAt = static_cast<std::uint64_t>(QDateTime::currentSecsSinceEpoch());

  if (!save()) return false;

  emit pinned(emoji);

  return true;
}

bool GlyphService::setSkinTone(std::string_view emoji, emoji::SkinTone tone) {
  auto &entry = entryFor(emoji);
  entry.skinTone = std::string(emoji::skinToneInfo(tone).id);

  if (!save()) return false;

  emit skintoneChanged(emoji);

  return true;
}

bool GlyphService::resetSkinTone(std::string_view emoji) {
  if (auto *entry = findEntry(emoji)) {
    entry->skinTone = std::nullopt;
    if (!save()) return false;
  }

  emit skintoneChanged(emoji);

  return true;
}

bool GlyphService::setKeywords(std::string_view emoji, const std::string &text) {
  auto &entry = entryFor(emoji);

  if (text.empty()) {
    entry.keyword.reset();
  } else {
    entry.keyword = text;
  }

  if (!save()) {
    qWarning() << "Failed to save";
    return false;
  }

  emit keywordsChanged(emoji);

  return true;
}
