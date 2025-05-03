#pragma once
#include "common.hpp"
#include "omni-database.hpp"
#include <chrono>
#include <qlogging.h>
#include <qsqlquery.h>
#include <qsqlerror.h>

static constexpr size_t RECENCY_WEIGHT = 2;
static constexpr size_t FREQUENCY_WEIGHT = 1;
static constexpr size_t FREQUENCY_CAP = 100;

class RankingService : public NonAssignable {
  struct RootSearchableItem {
    int visitedCount;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastVisitedAt;
    QString alias;
  };

  struct FrecencyRecord {
    int visitedCount;
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> lastVisitedAt;
  };

  using FrecencyMap = std::unordered_map<QString, FrecencyRecord>;

  FrecencyMap m_frecencyMap;
  OmniDatabase &m_db;
  QSqlQuery m_findRecord;
  QSqlQuery m_incrementQuery;
  QSqlQuery m_loadRecords;

  void createTables() {
    QSqlQuery query = m_db.createQuery();

    if (!query.exec(R"(
		CREATE TABLE IF NOT EXISTS frecency_ranked_item (
			id TEXT PRIMARY KEY,
			visited_count INT DEFAULT 0,
			reference_id TEXT,
			item_type TEXT,
			last_visited_at INT
		);
	)")) {
      qCritical() << "Failed to create frecency_ranked_item table" << query.lastError();
    }

    m_loadRecords = m_db.createQuery();
    m_loadRecords.prepare(R"(
		SELECT id, visited_count, last_visited_at FROM frecency_ranked_item WHERE item_type = :type
	)");

    m_incrementQuery = m_db.createQuery();
    if (!m_incrementQuery.prepare(R"(
		INSERT INTO frecency_ranked_item (id, item_type, visited_count, last_visited_at)
		VALUES (:id, :type, 1, (unixepoch()))
		ON CONFLICT (id) 
		DO UPDATE SET visited_count = visited_count + 1, last_visited_at = unixepoch()
		RETURNING visited_count, last_visited_at
	)")) {
      qCritical() << "Failed to prepare increment query" << m_incrementQuery.lastError();
    }
  }

public:
  FrecencyRecord registerVisit(const QString &type, const QString &id) {
    QString key = QString("%1:%2").arg(type).arg(id);

    qDebug() << "registering visit for" << key;

    m_incrementQuery.bindValue(":id", key);
    m_incrementQuery.bindValue(":type", type);

    if (!m_incrementQuery.exec() || !m_incrementQuery.next()) {
      qCritical() << "failed to register visit" << m_incrementQuery.lastError();
      return {};
    }

    FrecencyRecord record;

    record.visitedCount = m_incrementQuery.value(0).toUInt();
    std::time_t epoch = m_incrementQuery.value(1).toULongLong();
    record.lastVisitedAt = std::chrono::system_clock::from_time_t(epoch);
    m_frecencyMap.insert({key, record});
    m_incrementQuery.finish();

    return record;
  }

  const FrecencyRecord &findRecord(const QString &key) { return m_frecencyMap[key]; }

  double computeFrecency(const FrecencyRecord &record) {
    using namespace std::chrono;

    double recencyScore = 0;

    if (record.lastVisitedAt) {
      auto now = high_resolution_clock::now();
      double secondsSinceLastUse = duration_cast<seconds>(now - *record.lastVisitedAt).count();
      qDebug() << "recency" << secondsSinceLastUse;

      recencyScore = std::exp(-0.1 * secondsSinceLastUse);
    }

    double frequencyScore = std::min(1.0, static_cast<double>(record.visitedCount) / FREQUENCY_CAP);

    // qDebug() << "recency score" << m_entry.command->id() << recencyScore << "secs" << secondsSinceLastUse;

    return (recencyScore * RECENCY_WEIGHT) + (frequencyScore * FREQUENCY_WEIGHT);
  }

  void loadRecords(const QString &type) {
    m_loadRecords.bindValue(":type", type);

    if (!m_loadRecords.exec()) {
      qCritical() << "Failed to load frecency records of type" << type << m_loadRecords.lastError();
      return;
    }

    while (m_loadRecords.next()) {
      FrecencyRecord record;
      QString id = m_loadRecords.value(0).toString();
      QVariant lastVisitedAtField = m_loadRecords.value(2);

      record.visitedCount = m_loadRecords.value(1).toUInt();

      if (!lastVisitedAtField.isNull()) {
        std::time_t epoch = lastVisitedAtField.toULongLong();

        record.lastVisitedAt = std::chrono::system_clock::from_time_t(epoch);
      }

      m_frecencyMap.insert({id, record});
    }
  }

  RankingService(OmniDatabase &db) : m_db(db) { createTables(); }
};
