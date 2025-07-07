#include "calculator-service.hpp"
#include "omni-database.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include <qdatetime.h>
#include <qlogging.h>
#include <qsqlquery.h>
#include "services/calculator-service/qalculate/qalculate-backend.hpp"

using CalculatorRecord = CalculatorService::CalculatorRecord;

std::vector<CalculatorService::CalculatorRecord> CalculatorService::loadAll() const {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		SELECT
			id, type_hint, question, answer, created_at, pinned_at	
		FROM 
			calculator_history
		ORDER BY pinned_at DESC, created_at DESC
	)");

  if (!query.exec()) {
    qCritical() << "CalculatorService::loadAll() failed" << query.lastError();
    return {};
  }

  std::vector<CalculatorRecord> records;

  while (query.next()) {
    CalculatorRecord record;

    record.id = query.value(0).toInt();
    record.typeHint = static_cast<AbstractCalculatorBackend::CalculatorAnswerType>(query.value(1).toInt());
    record.question = query.value(2).toString();
    record.answer = query.value(3).toString();
    record.createdAt = QDateTime::fromSecsSinceEpoch(query.value(4).toULongLong());

    if (auto value = query.value(5); !value.isNull()) {
      record.pinnedAt = QDateTime::fromSecsSinceEpoch(value.toULongLong());
    }

    records.emplace_back(record);
  }

  return records;
}

std::vector<CalculatorRecord> CalculatorService::records() const { return m_records; }

AbstractCalculatorBackend *CalculatorService::backend() const { return m_backend.get(); }

bool CalculatorService::addRecord(const AbstractCalculatorBackend::CalculatorResult &result) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO calculator_history (type_hint, question, answer)
		VALUES (:type_hint, :question, :answer)
	)");
  query.addBindValue(result.type);
  query.addBindValue(result.question);
  query.addBindValue(result.answer);

  if (!query.exec()) {
    qCritical() << "Failed to add calculator record" << query.lastError();
    return false;
  }

  CalculatorRecord record;

  record.id = query.lastInsertId().toInt();
  record.question = result.question;
  record.answer = result.answer;
  record.typeHint = result.type;
  record.createdAt = QDateTime::currentDateTime();

  auto it = m_records.begin();

  while (it != m_records.end() && it->pinnedAt) {
    ++it;
  }

  m_records.insert(it, record);

  return true;
}

void CalculatorService::pinRecord(int id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE calculator_history SET pinned_at = unixepoch() WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to pin record with id" << id << query.lastError();
    return;
  }

  auto currentPos = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });

  currentPos->pinnedAt = QDateTime::currentDateTime();
  std::iter_swap(currentPos, m_records.begin());
}

void CalculatorService::unpinRecord(int id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE calculator_history SET pinned_at = NULL WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to unpin record with id" << id << query.lastError();
    return;
  }

  auto currentPos = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });

  currentPos->pinnedAt = std::nullopt;

  auto it = m_records.begin();

  while (it != m_records.end() && it->pinnedAt) {
    ++it;
  }

  if (it != m_records.end()) { std::iter_swap(currentPos, it); }
}

void CalculatorService::removeRecord(int id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("DELETE FROM calculator_history WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to remove record with id" << id << query.lastError();
    return;
  }

  auto it = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });

  if (it != m_records.end()) m_records.erase(it);
}

CalculatorService::CalculatorService(OmniDatabase &db) : m_db(db) {
  m_records = loadAll();
  m_backend = std::make_unique<QalculateBackend>();
}
