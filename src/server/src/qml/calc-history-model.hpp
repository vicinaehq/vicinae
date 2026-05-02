#pragma once
#include "section-source.hpp"
#include "services/calculator-service/calculator-service.hpp"
#include <vector>

using CalculatorRecord = CalculatorService::CalculatorRecord;

class CalcHistorySection : public SectionSource {
public:
  void setRecords(const QString &groupName, std::vector<CalculatorRecord> records);

  QString sectionName() const override { return m_groupName; }
  int count() const override { return static_cast<int>(m_records.size()); }

protected:
  QString itemTitle(int i) const override;
  QString itemIconSource(int i) const override;
  QVariantList itemAccessories(int i) const override;
  std::unique_ptr<ActionPanelState> actionPanel(int i) const override;

private:
  QString m_groupName;
  std::vector<CalculatorRecord> m_records;
};
