#include "calculator-service.hpp"
#include <quuid.h>
#include "fuzzy/fuzzy-searchable.hpp"
#include "omni-database.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include <ranges>
#include <qdatetime.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobjectdefs.h>

#include "qalculate/qalculate-backend.hpp"

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(BUNDLE_SOULVER_CORE)
#include "soulver-core/soulver-core.hpp"
#endif

using CalculatorRecord = CalculatorService::CalculatorRecord;

bool CalculatorService::setBackend(AbstractCalculatorBackend *newBackend) {
  if (m_backend == newBackend) return true;

  if (!newBackend->start()) {
    qWarning() << "Failed to start new calculator backend" << newBackend->id();
    return false;
  }

  qInfo() << "Started" << newBackend->displayName() << "calculator backend";
  newBackend->setDigitGroupingEnabled(m_digitGroupingEnabled);

  if (m_backend) { m_backend->stop(); }

  m_backend = newBackend;
  return true;
}

void CalculatorService::startFirstHealthy() {
  if (m_backend) m_backend->stop();

  for (const auto &backend : m_backends) {
    if (backend->start()) {
      qInfo() << "Started" << backend->displayName() << "calculator backend";
      backend->setDigitGroupingEnabled(m_digitGroupingEnabled);
      m_backend = backend.get();
      return;
    }
  }
}

bool CalculatorService::setBackend(const QString &id) {
  auto pred = [&](auto &&backend) { return backend->id() == id; };
  auto it = std::ranges::find_if(m_backends, pred);

  if (it == m_backends.end()) {
    qWarning() << "Trying to set non existent" << id << "as a calculator backend";
    return false;
  }

  return setBackend(it->get());
}

std::vector<CalculatorService::CalculatorRecord> CalculatorService::loadAll() const {
  auto stmt = m_db.db().prepare(R"(
    SELECT
      id, type_hint, question, answer, created_at, pinned_at
    FROM
      calculator_history
    ORDER BY pinned_at DESC, created_at DESC
  )");

  std::vector<CalculatorRecord> records;

  while (stmt.step()) {
    CalculatorRecord record;

    record.id = stmt.columnQString(0);
    record.typeHint = static_cast<AbstractCalculatorBackend::CalculatorAnswerType>(stmt.columnInt(1));
    record.question = stmt.columnQString(2);
    record.answer = stmt.columnQString(3);
    record.createdAt = QDateTime::fromSecsSinceEpoch(stmt.columnInt64(4));

    if (!stmt.isNull(5)) { record.pinnedAt = QDateTime::fromSecsSinceEpoch(stmt.columnInt64(5)); }

    records.emplace_back(record);
  }

  return records;
}

std::vector<CalculatorRecord> CalculatorService::records() const { return m_records; }

std::vector<CalculatorRecord> CalculatorService::query(const QString &query) {
  if (query.isEmpty()) return records();

  std::string const q = query.toStdString();
  std::vector<CalculatorRecord> results;

  for (const auto &record : records()) {
    auto question = record.question.toStdString();
    auto answer = record.answer.toStdString();
    if (fuzzy::scoreWeighted({{question, 1.0}, {answer, 0.5}}, q) > 0) { results.emplace_back(record); }
  }

  return results;
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
    QDate const startOfWeek(now.date().addDays(-(now.date().dayOfWeek() - 1)));
    QDate const endOfWeek(startOfWeek.addDays(7));

    groups.push_back({"This week", {}});

    for (; it != records.end() && it->createdAt >= startOfWeek.startOfDay() &&
           it->createdAt <= endOfWeek.startOfDay();
         ++it) {
      groups.back().second.emplace_back(*it);
    }
  }

  {
    QDate const startOfMonth(QDate(now.date().year(), now.date().month(), 1));
    QDateTime const endOfMonth = startOfMonth.addMonths(1).startOfDay();

    groups.push_back({"This month", {}});

    for (; it != records.end() && it->createdAt >= startOfMonth.startOfDay() && it->createdAt <= endOfMonth;
         ++it) {
      groups.back().second.emplace_back(*it);
    }
  }

  {

    QDate const startOfYear(now.date().year(), 1, 1);
    QDate const endOfYear(startOfYear.addYears(1));

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

AbstractCalculatorBackend *CalculatorService::backend() const { return m_backend; }

bool CalculatorService::addRecord(const AbstractCalculatorBackend::CalculatorResult &result) {
  auto stmt = m_db.db().prepare(R"(
    INSERT INTO calculator_history (id, type_hint, question, answer, created_at)
    VALUES (:id, :type_hint, :question, :answer, :epoch)
  )");

  auto id = QUuid::createUuid().toString(QUuid::WithoutBraces);

  stmt.bind(":id", id);
  stmt.bind(":type_hint", static_cast<int>(result.type));
  stmt.bind(":question", result.question.text);
  stmt.bind(":answer", result.answer.text);
  stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) {
    qCritical() << "Failed to add calculator record" << stmt.lastError().c_str();
    return false;
  }

  CalculatorRecord record;

  record.id = id;
  record.question = result.question.text;
  record.answer = result.answer.text;
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
  auto stmt = m_db.db().prepare("UPDATE calculator_history SET pinned_at = :epoch WHERE id = :id");
  stmt.bind(":id", id);
  stmt.bind(":epoch", static_cast<int64_t>(QDateTime::currentSecsSinceEpoch()));

  if (!stmt.exec()) {
    qCritical() << "Failed to pin record with id" << id << stmt.lastError().c_str();
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
  auto stmt = m_db.db().prepare("UPDATE calculator_history SET pinned_at = NULL WHERE id = :id");
  stmt.bind(":id", id);

  if (!stmt.exec()) {
    qCritical() << "Failed to unpin record with id" << id << stmt.lastError().c_str();
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
  auto stmt = m_db.db().prepare("DELETE FROM calculator_history WHERE id = :id");
  stmt.bind(":id", id);

  if (!stmt.exec()) {
    qCritical() << "Failed to remove record with id" << id << stmt.lastError().c_str();
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

void CalculatorService::setDigitGroupingEnabled(bool value) {
  m_digitGroupingEnabled = value;
  if (m_backend) { m_backend->setDigitGroupingEnabled(value); }
}

bool CalculatorService::removeAll() {
  if (!m_db.db().exec("DELETE FROM calculator_history")) {
    qCritical() << "removeAll: failed" << m_db.db().lastError().c_str();
    return false;
  }

  return true;
}

void CalculatorService::updateConversionRecords() {
  auto tx = m_db.db().transaction();

  auto stmt =
      m_db.db().prepare("UPDATE calculator_history SET answer = :answer, type_hint = :type WHERE id = :id");

  auto isConversionRecord = [](const CalculatorRecord &record) {
    return record.typeHint == AbstractCalculatorBackend::CONVERSION;
  };

  for (auto &record : m_records | std::views::filter(isConversionRecord)) {
    auto result = m_backend->compute(record.question, {.mode = AbstractCalculatorBackend::ComputeMode::Full});

    if (!result) continue;

    stmt.reset();
    stmt.bind(":answer", result->answer.text);
    stmt.bind(":type", static_cast<int>(result->type));
    stmt.bind(":id", record.id);

    if (!stmt.exec()) { qCritical() << "Failed to update conversion record" << stmt.lastError().c_str(); }

    record.answer = result->answer.text;
    record.typeHint = result->type;
  }

  if (!tx.commit()) {
    qCritical() << "updateConversionRecords: failed to commit transaction";
    return;
  }

  emit conversionRecordsUpdated();
}

const std::vector<std::unique_ptr<AbstractCalculatorBackend>> &CalculatorService::backends() const {
  return m_backends;
}

CalculatorService::CalculatorService(OmniDatabase &db) : m_db(db) {
  m_records = loadAll();

  {
    std::vector<std::unique_ptr<AbstractCalculatorBackend>> candidates;

#if defined(Q_OS_MACOS) && defined(BUNDLE_SOULVER_CORE)
    candidates.emplace_back(std::make_unique<SoulverCoreCalculator>());
#endif
    candidates.emplace_back(std::make_unique<QalculateBackend>());
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    candidates.emplace_back(std::make_unique<SoulverCoreCalculator>());
#endif

    for (auto &candidate : candidates) {
      if (candidate->isActivatable()) { m_backends.emplace_back(std::move(candidate)); }
    }
  }
}
