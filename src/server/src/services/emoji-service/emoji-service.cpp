#include "emoji-service.hpp"
#include "fuzzy/fzf.hpp"
#include "omni-database.hpp"
#include <qlogging.h>
#include "utils/utils.hpp"

void EmojiService::loadKeywords() {
  for (const auto &visited : getVisited()) {
    if (!visited.keywords.isEmpty()) {
      m_keywordMap.insert({std::string(visited.data->emoji), visited.keywords.toStdString()});
    }
  }
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

void EmojiService::createDbEntry(std::string_view emoji) {
  auto stmt =
      m_db.db().prepare("INSERT INTO visited_emoji (emoji) VALUES (:emoji) ON CONFLICT(emoji) DO NOTHING");
  stmt.bind(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));

  if (!stmt.exec()) {
    qCritical() << "Failed to create database entry for emoji" << stmt.lastError().c_str();
  }
}

bool EmojiService::registerVisit(std::string_view emoji) {
  auto stmt = m_db.db().prepare(R"(
    INSERT INTO visited_emoji (emoji, visit_count, last_visited_at)
    VALUES (:emoji, 1, :epoch)
    ON CONFLICT(emoji) DO UPDATE
    SET
      visit_count = visit_count + 1,
      last_visited_at = :epoch
  )");
  stmt.bind(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));
  stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) {
    qCritical() << "Failed to register visit for emoji" << stmt.lastError().c_str();
    return false;
  }

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
  auto stmt = m_db.db().prepare(R"(
    SELECT emoji, visit_count, pinned_at, custom_keywords FROM visited_emoji ORDER BY pinned_at DESC, visit_count DESC, last_visited_at DESC
  )");

  std::vector<EmojiWithMetadata> results;
  const auto &mapping = StaticEmojiDatabase::mapping();

  while (stmt.step()) {
    EmojiWithMetadata result;
    auto emoji = stmt.columnQString(0);
    auto it = mapping.find(emoji.toStdString());

    if (it == mapping.end()) {
      qWarning() << "Emoji is not in the mapping" << emoji;
      continue;
    }

    result.data = it->second;
    result.visitCount = static_cast<uint32_t>(stmt.columnInt(1));
    result.keywords = stmt.columnQString(3);

    if (!stmt.isNull(2)) { result.pinnedAt = QDateTime::fromSecsSinceEpoch(stmt.columnInt64(2)); }

    results.emplace_back(result);
  }

  return results;
}

std::vector<EmojiWithMetadata> EmojiService::mapMetadata(const std::vector<const EmojiData *> &items) {
  if (items.empty()) return {};

  std::unordered_map<std::string_view, EmojiWithMetadata> visitMap;

  for (const auto &visited : getVisited()) {
    visitMap.insert({visited.data->emoji, visited});
  }

  std::vector<EmojiWithMetadata> metadatas;

  metadatas.reserve(items.size());

  for (const auto &item : items) {
    if (auto it = visitMap.find(item->emoji); it != visitMap.end()) {
      metadatas.emplace_back(it->second);
    } else {
      metadatas.emplace_back(EmojiWithMetadata{.data = item});
    }
  }

  return metadatas;
}

EmojiWithMetadata EmojiService::mapMetadata(std::string_view emoji) {
  auto it = StaticEmojiDatabase::mapping().find(emoji);

  if (it == StaticEmojiDatabase::mapping().end()) { return {}; }

  auto stmt = m_db.db().prepare(
      "SELECT visit_count, pinned_at, custom_keywords FROM visited_emoji WHERE emoji = :emoji");
  stmt.bind(":emoji", qStringFromStdView(emoji));

  if (!stmt.step()) { return {.data = it->second}; }

  EmojiWithMetadata result;

  result.data = it->second;
  result.visitCount = static_cast<uint32_t>(stmt.columnInt(0));
  result.keywords = stmt.columnQString(2);

  if (!stmt.isNull(1)) { result.pinnedAt = QDateTime::fromSecsSinceEpoch(stmt.columnInt64(1)); }

  return result;
}

bool EmojiService::setCustomKeywords(std::string_view emoji, const QString &keywords) {
  auto oldMetadata = mapMetadata(emoji);

  createDbEntry(emoji);

  auto stmt = m_db.db().prepare("UPDATE visited_emoji SET custom_keywords = :keywords WHERE emoji = :emoji");
  stmt.bind(":keywords", keywords);
  stmt.bind(":emoji", qStringFromStdView(emoji));

  if (!stmt.exec()) {
    qCritical() << "Failed to setCustomKeywords for emoji" << stmt.lastError().c_str();
    return false;
  }

  m_keywordMap[keywords.toStdString()] = keywords.toStdString();

  return true;
}

bool EmojiService::resetRanking(std::string_view emoji) {
  createDbEntry(emoji);

  auto stmt = m_db.db().prepare(
      "UPDATE visited_emoji SET visit_count = 0, last_visited_at = NULL WHERE emoji = :emoji");
  stmt.bind(":emoji", qStringFromStdView(emoji));

  if (!stmt.exec()) {
    qCritical() << "Failed to reset ranking" << stmt.lastError().c_str();
    return false;
  }

  emit rankingReset(emoji);

  return true;
}

bool EmojiService::unpin(std::string_view emoji) {
  createDbEntry(emoji);

  auto stmt = m_db.db().prepare("UPDATE visited_emoji SET pinned_at = NULL WHERE emoji = :emoji");
  stmt.bind(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));

  if (!stmt.exec()) {
    qCritical() << "Failed to pin emoji";
    return false;
  }

  emit unpinned(emoji);

  return true;
}

bool EmojiService::pin(std::string_view emoji) {
  createDbEntry(emoji);

  auto stmt = m_db.db().prepare("UPDATE visited_emoji SET pinned_at = :epoch WHERE emoji = :emoji");
  stmt.bind(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));
  stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) {
    qCritical() << "Failed to pin emoji";
    return false;
  }

  emit pinned(emoji);

  return true;
}

EmojiService::EmojiService(OmniDatabase &db) : m_db(db) { loadKeywords(); }
