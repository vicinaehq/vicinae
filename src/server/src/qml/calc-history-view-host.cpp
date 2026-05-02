#include "calc-history-view-host.hpp"
#include "service-registry.hpp"

void CalcHistoryViewHost::initialize() {
  BaseView::initialize();
  initModel();

  m_calc = context()->services->calculatorService();

  setSearchPlaceholderText("Search past calculations...");

  connect(m_calc, &CalculatorService::recordPinned, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::recordUnpinned, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::recordRemoved, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::allRecordsRemoved, this, &CalcHistoryViewHost::refresh);
}

void CalcHistoryViewHost::loadInitialData() { textChanged(searchText()); }

void CalcHistoryViewHost::textChanged(const QString &text) {
  m_query = text;
  auto data = m_calc->groupRecordsByTime(m_calc->query(text));
  applyGroupedData(std::move(data));
}

void CalcHistoryViewHost::refresh() { textChanged(m_query); }

void CalcHistoryViewHost::applyGroupedData(CalculatorService::GroupedRecordList data) {
  model()->clearSources();
  m_sections.clear();
  m_sections.reserve(data.size());

  for (auto &[name, records] : data) {
    if (records.empty()) continue;
    auto section = std::make_unique<CalcHistorySection>();
    section->setRecords(name, std::move(records));
    model()->addSource(section.get());
    m_sections.emplace_back(std::move(section));
  }

  model()->rebuild();
}
