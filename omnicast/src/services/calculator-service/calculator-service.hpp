#pragma once
#include "omni-database.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include <qdatetime.h>

class CalculatorService {
public:
  struct CalculatorRecord {
    int id;
    QString question;
    QString answer;
    AbstractCalculatorBackend::CalculatorAnswerType typeHint;
    QDateTime createdAt;
    std::optional<QDateTime> pinnedAt;
  };

private:
  OmniDatabase &m_db;
  std::vector<CalculatorRecord> m_records;
  std::unique_ptr<AbstractCalculatorBackend> m_backend;
  std::vector<CalculatorRecord> loadAll() const;

public:
  AbstractCalculatorBackend *backend() const;

  std::vector<CalculatorRecord> records() const;
  bool addRecord(const AbstractCalculatorBackend::CalculatorResult &result);
  void removeRecord(int id);
  void pinRecord(int id);
  void unpinRecord(int id);

  CalculatorService(OmniDatabase &db);
};
