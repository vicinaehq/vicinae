#include "emoji-service.hpp"
#include "omni-database.hpp"
#include "services/emoji-service/emoji.hpp"
#include <qlogging.h>
#include "utils/utils.hpp"
#include <qsqlquery.h>

void EmojiService::buildIndex() {
  std::unordered_map<std::string_view, QString> keywordMap;

  for (const auto &visited : getVisited()) {
    if (!visited.keywords.isEmpty()) { keywordMap.insert({visited.data->emoji, visited.keywords}); }
  }

  for (const auto &emoji : StaticEmojiDatabase::orderedList()) {
    if (auto it = keywordMap.find(emoji.emoji); it != keywordMap.end()) {
      m_index.indexLatinText(it->second.toStdString(), &emoji);
    }

    for (const auto &keyword : emoji.keywords) {
      m_index.indexLatinText(keyword, &emoji);
    }
  }
}

std::vector<const EmojiData *> EmojiService::search(std::string_view query) const {
  return m_index.prefixSearch(query);
}

void EmojiService::createDbEntry(std::string_view emoji) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("INSERT INTO visited_emoji (emoji) VALUES (:emoji) ON CONFLICT(emoji) DO NOTHING");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) { qCritical() << "Failed to create database entry for emoji" << query.lastError(); }
}

bool EmojiService::registerVisit(std::string_view emoji) {
  QSqlQuery query = m_db.createQuery();
  qint64 epoch = QDateTime::currentSecsSinceEpoch();

  query.prepare(R"(
  	INSERT INTO visited_emoji (emoji, visit_count, last_visited_at)
	VALUES (:emoji, 1, :epoch)
	ON CONFLICT(emoji) DO UPDATE 
	SET 
		visit_count = visit_count + 1, 
		last_visited_at = :epoch
	)");
  query.bindValue(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));
  query.bindValue(":epoch", epoch);

  if (!query.exec()) {
    qCritical() << "Failed to register visit for emoji" << query.lastError();
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
    grouped.push_back({group, std::move(map[group])});
  }

  return grouped;
}

std::vector<EmojiWithMetadata> EmojiService::getVisited() const {
  QSqlQuery query = m_db.createQuery();

  bool ok = query.exec(R"(
	SELECT emoji, visit_count, pinned_at, custom_keywords FROM visited_emoji ORDER BY pinned_at DESC, visit_count DESC, last_visited_at DESC
  )");

  if (!ok) {
    qCritical() << "Failed to getVisited()" << query.lastError();
    return {};
  }

  std::vector<EmojiWithMetadata> results;
  const auto &mapping = StaticEmojiDatabase::mapping();

  while (query.next()) {
    EmojiWithMetadata result;
    auto emoji = query.value(0).toString();
    auto it = mapping.find(emoji.toStdString());

    if (it == mapping.end()) {
      qWarning() << "Emoji is not in the mapping" << emoji;
      continue;
    }

    result.data = it->second;
    result.visitCount = query.value(1).toUInt();
    result.keywords = query.value(3).toString();

    if (auto value = query.value(2); !value.isNull()) {
      result.pinnedAt = QDateTime::fromSecsSinceEpoch(value.toULongLong());
    }

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
  QSqlQuery query = m_db.createQuery();
  auto it = StaticEmojiDatabase::mapping().find(emoji);

  if (it == StaticEmojiDatabase::mapping().end()) { return {}; }

  query.prepare("SELECT visit_count, pinned_at, custom_keywords FROM visited_emoji WHERE emoji = :emoji");
  query.addBindValue(qStringFromStdView(emoji));

  if (!query.exec()) {
    qCritical() << "Failed to map metadata for" << query.lastError();
    return {.data = it->second};
  }

  if (!query.next()) { return EmojiWithMetadata{.data = it->second}; }

  EmojiWithMetadata result;

  result.data = it->second;
  result.visitCount = query.value(0).toUInt();
  result.keywords = query.value(2).toString();

  if (auto value = query.value(1); !value.isNull()) {
    result.pinnedAt = QDateTime::fromSecsSinceEpoch(value.toULongLong());
  }

  return result;
}

bool EmojiService::setCustomKeywords(std::string_view emoji, const QString &keywords) {
  QSqlQuery query = m_db.createQuery();
  auto oldMetadata = mapMetadata(emoji);

  createDbEntry(emoji);

  query.prepare("UPDATE visited_emoji SET custom_keywords = :keywords WHERE emoji = :emoji");
  query.addBindValue(keywords);
  query.addBindValue(qStringFromStdView(emoji));

  if (!query.exec()) {
    qCritical() << "Failed to setCustomKeywords for emoji" << query.lastError();
    return false;
  }

  // hot reload index

  if (oldMetadata.data && !oldMetadata.keywords.isEmpty()) {
    m_index.removeLatinTextItem(oldMetadata.keywords.toStdString(), oldMetadata.data);
  }

  m_index.indexLatinText(keywords.toStdString(), oldMetadata.data);

  return true;
}

bool EmojiService::resetRanking(std::string_view emoji) {
  createDbEntry(emoji);

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE visited_emoji SET visit_count = 0, last_visited_at = NULL WHERE emoji = :emoji");
  query.addBindValue(qStringFromStdView(emoji));

  if (!query.exec()) {
    qCritical() << "Failed to reset ranking" << query.lastError();
    return false;
  }

  emit rankingReset(emoji);

  return true;
}

bool EmojiService::unpin(std::string_view emoji) {
  createDbEntry(emoji);

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE visited_emoji SET pinned_at = NULL WHERE emoji = :emoji");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) {
    qCritical() << "Failed to pin emoji";
    return false;
  }

  emit unpinned(emoji);

  return true;
}

bool EmojiService::pin(std::string_view emoji) {
  createDbEntry(emoji);

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE visited_emoji SET pinned_at = :epoch WHERE emoji = :emoji");
  query.bindValue(":emoji", QString::fromUtf8(emoji.data(), emoji.size()));
  query.bindValue(":epoch", QDateTime::currentSecsSinceEpoch());

  if (!query.exec()) {
    qCritical() << "Failed to pin emoji";
    return false;
  }

  emit pinned(emoji);

  return true;
}

EmojiService::EmojiService(OmniDatabase &db) : m_db(db) { buildIndex(); }
