#pragma once
#include "calc-history-model.hpp"
#include "list-view-host.hpp"
#include <memory>
#include <vector>

class CalculatorService;

class CalcHistoryViewHost : public ListViewHost {
  Q_OBJECT

public:
  void initialize() override;
  void loadInitialData() override;
  void textChanged(const QString &text) override;

private:
  void refresh();
  void applyGroupedData(CalculatorService::GroupedRecordList data);

  CalculatorService *m_calc = nullptr;
  QString m_query;
  std::vector<std::unique_ptr<CalcHistorySection>> m_sections;
};
