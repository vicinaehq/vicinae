#include "emoji-service.hpp"
#include "omni-database.hpp"
#include "services/emoji-service/emoji.hpp"
#include <qlogging.h>
#include <qsqlquery.h>

void EmojiService::buildIndex() {
  for (const auto &emoji : StaticEmojiDatabase::orderedList()) {
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

  query.prepare("INSERT INTO visited_emoji (emoji) VALUES (:emoji)");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) { qCritical() << "Failed to create database entry for emoji" << query.lastError(); }
}

bool EmojiService::registerVisit(std::string_view emoji) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
  	INSERT INTO visited_emoji (emoji, visit_count, last_visited_at)
	VALUES (:emoji, 1, (unixepoch()))
	ON CONFLICT(emoji) DO UPDATE 
	SET 
		visit_count = visit_count + 1, 
		last_visited_at = unixepoch()
	)");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) {
    qCritical() << "Failed to register visit for emoji" << emoji << query.lastError();
    return false;
  }

  return true;
}

std::vector<EmojiWithMetadata> EmojiService::getVisited() const {
  QSqlQuery query = m_db.createQuery();

  bool ok = query.exec(R"(
	SELECT emoji, visit_count, pinned_at FROM visited_emoji ORDER BY pinned_at DESC, visit_count DESC, last_visited_at DESC
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

    if (auto value = query.value(2); !value.isNull()) {
      result.pinnedAt = QDateTime::fromSecsSinceEpoch(value.toULongLong());
    }

    results.emplace_back(result);
  }

  return results;
}

bool EmojiService::unpin(std::string_view emoji) {
  createDbEntry(emoji);

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE visited_emoji SET pinned_at = NULL WHERE emoji = :emoji");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) {
    qCritical() << "Failed to pin emoji" << emoji;
    return false;
  }

  return true;
}

bool EmojiService::pin(std::string_view emoji) {
  createDbEntry(emoji);

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE visited_emoji SET pinned_at = unixepoch() WHERE emoji = :emoji");
  query.addBindValue(QString::fromUtf8(emoji.data(), emoji.size()));

  if (!query.exec()) {
    qCritical() << "Failed to pin emoji" << emoji;
    return false;
  }

  return true;
}

EmojiService::EmojiService(OmniDatabase &db) : m_db(db) { buildIndex(); }
