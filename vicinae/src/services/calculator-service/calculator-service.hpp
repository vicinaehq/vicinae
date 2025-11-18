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

    QString id;
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
  AbstractCalculatorBackend *m_backend = nullptr;
  std::vector<std::unique_ptr<AbstractCalculatorBackend>> m_backends;

  std::vector<CalculatorRecord> loadAll() const;
  bool m_updateConversionsAfterRateUpdate = true;
  bool setBackend(AbstractCalculatorBackend *backend);

public:
  AbstractCalculatorBackend *backend() const;

  void startFirstHealthy();
  void setUpdateConversionsAfterRateUpdate(bool value);
  std::vector<CalculatorRecord> records() const;
  std::vector<std::pair<QString, std::vector<CalculatorRecord>>>
  groupRecordsByTime(const std::vector<CalculatorRecord> &records) const;
  bool addRecord(const AbstractCalculatorBackend::CalculatorResult &result);
  std::vector<CalculatorRecord> query(const QString &query);
  bool removeRecord(const QString &id);
  bool pinRecord(const QString &id);
  bool unpinRecord(const QString &id);
  bool removeAll();

  void updateConversionRecords();

  /**
   * Set the calculator backend to use.
   * If the specified backend is different than the one currently running, the current one
   * will be stopped (and notified) before the new one can be started;
   *
   * returns whether the new backend was successfully set
   */
  bool setBackend(const QString &id);

  /**
   * The list of activatable backends.
   */
  const std::vector<std::unique_ptr<AbstractCalculatorBackend>> &backends() const;

  CalculatorService(OmniDatabase &db);

signals:
  void conversionRecordsUpdated();
  void allRecordsRemoved() const;
  void recordAdded(const QString &id) const;
  void recordRemoved(const QString &id) const;
  void recordPinned(const QString &id) const;
  void recordUnpinned(const QString &id) const;
};
