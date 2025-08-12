#include "calculator-service.hpp"
#include "crypto.hpp"
#include "omni-database.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include <ranges>
#include <qdatetime.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
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

    record.id = query.value(0).toString();
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

std::vector<CalculatorRecord> CalculatorService::query(const QString &query) {
  /**
   * For now, we scan the entire list linearly as it will be fast enough for 99% of use cases.
   * We could consider using a proper in-memory index if that ever becomes a problem.
   */

  auto filter = [&query](const CalculatorRecord &record) {
    return record.question.contains(query, Qt::CaseInsensitive) ||
           record.answer.contains(query, Qt::CaseInsensitive);
  };

  return records() | std::views::filter(filter) | std::ranges::to<std::vector>();
}

std::vector<std::pair<QString, std::vector<CalculatorRecord>>>
CalculatorService::groupRecordsByTime(const std::vector<CalculatorRecord> &records) const {
  std::vector<std::pair<QString, std::vector<CalculatorRecord>>> groups;
  auto now = QDateTime::currentDateTime();
  auto it = records.begin();
  static const std::vector<std::pair<QString, int>> dividers = {
      {"Today", 1}, {"A week ago", 7}, {"A month ago", 30}, {"A year ago", 365}};

  groups.reserve(dividers.size() + 2);
  groups.push_back({"Pinned", {}});

  for (; it != records.end() && it->pinnedAt; ++it) {
    groups.back().second.emplace_back(*it);
  }

  now.date().startOfDay();
  groups.push_back({"Today", {}});

  for (; it != records.end() && it->createdAt >= now.date().startOfDay() &&
         it->createdAt <= now.date().endOfDay();
       ++it) {
    groups.back().second.emplace_back(*it);
  }

  {
    QDate startOfWeek(now.date().addDays(-(now.date().dayOfWeek() - 1)));
    QDate endOfWeek(startOfWeek.addDays(7));

    groups.push_back({"This week", {}});

    for (; it != records.end() && it->createdAt >= startOfWeek.startOfDay() &&
           it->createdAt <= endOfWeek.startOfDay();
         ++it) {
      groups.back().second.emplace_back(*it);
    }
  }

  {
    QDate startOfMonth(QDate(now.date().year(), now.date().month(), 1));
    QDateTime endOfMonth = startOfMonth.addMonths(1).startOfDay();

    groups.push_back({"This month", {}});

    for (; it != records.end() && it->createdAt >= startOfMonth.startOfDay() && it->createdAt <= endOfMonth;
         ++it) {
      groups.back().second.emplace_back(*it);
    }
  }

  {

    QDate startOfYear(now.date().year(), 1, 1);
    QDate endOfYear(startOfYear.addYears(1));

    groups.push_back({"This year", {}});

    for (; it != records.end() && it->createdAt >= startOfYear.startOfDay() &&
           it->createdAt <= endOfYear.startOfDay();
         ++it) {
      groups.back().second.emplace_back(*it);
    }
  }

  groups.push_back({"A few years ago", {}});

  for (; it != records.end(); ++it) {
    groups.back().second.emplace_back(*it);
  }

  return groups;
}

AbstractCalculatorBackend *CalculatorService::backend() const { return m_backend.get(); }

bool CalculatorService::addRecord(const AbstractCalculatorBackend::CalculatorResult &result) {
  QSqlQuery query = m_db.createQuery();

  query.prepare(R"(
		INSERT INTO calculator_history (id, type_hint, question, answer)
		VALUES (:id, :type_hint, :question, :answer)
	)");
  query.bindValue(":id", Crypto::UUID::v4());
  query.bindValue(":type_hint", result.type);
  query.bindValue(":question", result.question);
  query.bindValue(":answer", result.answer);

  if (!query.exec()) {
    qCritical() << "Failed to add calculator record" << query.lastError();
    return false;
  }

  CalculatorRecord record;

  record.id = query.lastInsertId().toString();
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

bool CalculatorService::pinRecord(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE calculator_history SET pinned_at = unixepoch() WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to pin record with id" << id << query.lastError();
    return false;
  }

  auto currentPos = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });
  auto record = *currentPos;

  record.pinnedAt = QDateTime::currentDateTime();
  m_records.erase(currentPos);
  m_records.insert(m_records.begin(), record);
  emit recordPinned(id);

  return true;
}

bool CalculatorService::unpinRecord(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE calculator_history SET pinned_at = NULL WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to unpin record with id" << id << query.lastError();
    return false;
  }

  auto currentPos = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });
  auto record = *currentPos;

  auto newPos = m_records.begin();

  while (newPos != m_records.end() && newPos->pinnedAt) {
    ++newPos;
  }

  while (newPos != m_records.end() && newPos->createdAt > currentPos->createdAt) {
    ++newPos;
  }

  m_records.erase(currentPos);
  record.pinnedAt = std::nullopt;
  m_records.insert(newPos, record);

  emit recordUnpinned(id);

  return true;
}

bool CalculatorService::removeRecord(const QString &id) {
  QSqlQuery query = m_db.createQuery();

  query.prepare("DELETE FROM calculator_history WHERE id = :id");
  query.addBindValue(id);

  if (!query.exec()) {
    qCritical() << "Failed to remove record with id" << id << query.lastError();
    return false;
  }

  auto it = std::ranges::find_if(m_records, [&](auto &&rec) { return rec.id == id; });

  if (it != m_records.end()) m_records.erase(it);

  emit recordRemoved(id);

  return true;
}

void CalculatorService::setUpdateConversionsAfterRateUpdate(bool value) {
  m_updateConversionsAfterRateUpdate = value;
}

bool CalculatorService::refreshExchangeRates() {
  if (!m_backend->reloadExchangeRates()) { return false; }
  if (m_updateConversionsAfterRateUpdate) { updateConversionRecords(); }

  return true;
}

bool CalculatorService::removeAll() {
  QSqlQuery query = m_db.createQuery();

  if (!query.exec("DELETE FROM calculator_history")) {
    qCritical() << "removeAll: failed" << query.lastError();
    return false;
  }

  return true;
}

void CalculatorService::updateConversionRecords() {
  if (!m_db.db().transaction()) {
    qCritical() << "updateConversionRecords: failed to start transaction";
    return;
  }

  QSqlQuery query = m_db.createQuery();

  query.prepare("UPDATE calculator_history SET answer = :answer, type_hint = :type WHERE id = :id");

  auto isConversionRecord = [](const CalculatorRecord &record) {
    return record.typeHint == AbstractCalculatorBackend::CONVERSION;
  };

  for (auto &record : m_records | std::views::filter(isConversionRecord)) {
    auto result = m_backend->compute(record.question);

    if (!result) continue;

    query.bindValue(":answer", result->answer);
    query.bindValue(":type", result->type);
    query.bindValue(":id", record.id);

    if (!query.exec()) { qCritical() << "Failed to update conversion record" << query.lastError(); }

    record.answer = result->answer;
    record.typeHint = result->type;
  }

  if (!m_db.db().commit()) {
    qCritical() << "updateConversionRecords: failed to commit transaction";
    return;
  }

  emit conversionRecordsUpdated();
}

CalculatorService::CalculatorService(OmniDatabase &db) : m_db(db) {
  m_records = loadAll();
  /**
   * We are doing proper backend abstraction, but for now it is not planned to add alternative backends.
   * libqalculate is very complete and will probably support most of our future needs.
   */
  m_backend = std::make_unique<QalculateBackend>();
}
