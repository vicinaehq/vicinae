#pragma once
#include "services/calculator-service/calculator-service.hpp"
#include "calculator-history-model.hpp"
#include <qobject.h>

class CalculatorHistoryController : public QObject {
public:
  CalculatorHistoryController(CalculatorService *calculator, CalculatorHistoryModel *model,
                              QObject *parent = nullptr)
      : QObject(parent), m_calc(calculator), m_model(model) {
    connect(m_calc, &CalculatorService::recordPinned, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::recordUnpinned, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::recordRemoved, this, &CalculatorHistoryController::refresh);
    connect(m_calc, &CalculatorService::allRecordsRemoved, this, &CalculatorHistoryController::refresh);
  }

  void setFilter(const QString &query) {
    m_query = query;
    auto grouped = m_calc->groupRecordsByTime(m_calc->query(query));
    m_model->setGroupedRecords(grouped);
  }

protected:
  void refresh() { setFilter(m_query); }

private:
  QString m_query;
  CalculatorService *m_calc;
  CalculatorHistoryModel *m_model;
};
