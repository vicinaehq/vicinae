#pragma once
#include "omni-database.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <qdatetime.h>
#include <qobject.h>
#include <qtmetamacros.h>

/**
 * Service used for everything calculator, including performing actual calculation (using the configured
 * backend), and history management.
 *
 * Since the calculator history is not expected to grow tremendously big,
 * every record is loaded in memory at startup and CRUD operations are directly modifying the in memory list
 * of records as well as the underlying sqlite database. The memory overhead of this is negligible, even for
 * tens of thousands entries. We could optimize this later in many ways if that ever becomes an issue.
 */

class CalculatorService : public QObject {
  Q_OBJECT

public:
  struct CalculatorRecord {
    QString expression() const { return QString("%1 = %2").arg(question).arg(answer); }

    int id;
    QString question;
    QString answer;
    /**
     * A hint as to what kind of calculation this is. This can be used by the service to make particular
     * decisions. For instance, the CONVERSION type hint is used to specifically target conversion
     * calculations that may need to be reevaluated after exchange rates got updated.
     */
    AbstractCalculatorBackend::CalculatorAnswerType typeHint;
    QDateTime createdAt;
    std::optional<QDateTime> pinnedAt;
  };

private:
  OmniDatabase &m_db;
  std::vector<CalculatorRecord> m_records;
  std::unique_ptr<AbstractCalculatorBackend> m_backend;
  std::vector<CalculatorRecord> loadAll() const;
  bool m_updateConversionsAfterRateUpdate = true;

public:
  AbstractCalculatorBackend *backend() const;

  void setUpdateConversionsAfterRateUpdate(bool value);
  std::vector<CalculatorRecord> records() const;
  std::vector<std::pair<QString, std::vector<CalculatorRecord>>>
  groupRecordsByTime(const std::vector<CalculatorRecord> &records) const;
  bool addRecord(const AbstractCalculatorBackend::CalculatorResult &result);
  std::vector<CalculatorRecord> query(const QString &query);
  bool removeRecord(int id);
  bool pinRecord(int id);
  bool unpinRecord(int id);
  bool removeAll();

  void updateConversionRecords();

  /**
   * Refresh exchange rates if the backend supports it.
   * If m_updateConversionsAfterRateUpdate is set to true, records with a type hint of CONVERSION
   * will be updated to reflect the new rates.
   */
  bool refreshExchangeRates();

  CalculatorService(OmniDatabase &db);

signals:
  void conversionRecordsUpdated();
  void allRecordsRemoved() const;
  void recordAdded(int id) const;
  void recordRemoved(int id) const;
  void recordPinned(int id) const;
  void recordUnpinned(int id) const;
};
