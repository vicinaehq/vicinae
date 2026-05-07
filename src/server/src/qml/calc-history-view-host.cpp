#include "calc-history-view-host.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"

using namespace std::chrono_literals;

void CalcLiveSection::setResult(std::optional<AbstractCalculatorBackend::CalculatorResult> result) {
  m_result = std::move(result);
  notifyChanged();
}

void CalcLiveSection::clear() {
  if (!m_result) return;
  m_result.reset();
  notifyItemsRefreshed();
}

QString CalcLiveSection::itemTitle(int) const {
  if (!m_result) return {};
  return m_result->question.text + QStringLiteral(" = ") + m_result->answer.text;
}

QString CalcLiveSection::itemIconSource(int) const { return imageSourceFor(ImageURL::builtin("calculator")); }

QVariantList CalcLiveSection::itemAccessories(int) const { return {}; }

QHash<int, QByteArray> CalcLiveSection::customRoleNames() const {
  return {
      {IsCalculator, "isCalculator"},         {CalcQuestion, "calcQuestion"},
      {CalcQuestionUnit, "calcQuestionUnit"}, {CalcAnswer, "calcAnswer"},
      {CalcAnswerUnit, "calcAnswerUnit"},
  };
}

QHash<int, QVariant> CalcLiveSection::customRoleDefaults() const {
  return {
      {IsCalculator, false},   {CalcQuestion, QString()},   {CalcQuestionUnit, QString()},
      {CalcAnswer, QString()}, {CalcAnswerUnit, QString()},
  };
}

QVariant CalcLiveSection::customData(int, int role) const {
  if (!m_result) {
    if (role == IsCalculator) return false;
    return {};
  }
  switch (role) {
  case IsCalculator:
    return true;
  case CalcQuestion:
    return m_result->question.text;
  case CalcQuestionUnit:
    return m_result->question.unit ? m_result->question.unit->displayName : QString();
  case CalcAnswer:
    return m_result->answer.text;
  case CalcAnswerUnit:
    return m_result->answer.unit ? m_result->answer.unit->displayName : QString();
  default:
    return {};
  }
}

std::unique_ptr<ActionPanelState> CalcLiveSection::actionPanel(int) const {
  if (!m_result) return nullptr;

  auto panel = std::make_unique<ListActionPanelState>();
  auto *main = panel->createSection();
  auto *copyAnswer = new CopyCalculatorAnswerAction(*m_result);
  copyAnswer->setPrimary(true);
  main->addAction(copyAnswer);
  main->addAction(new CopyCalculatorQuestionAndAnswerAction(*m_result));

  return panel;
}

void CalcHistoryViewHost::initialize() {
  BaseView::initialize();
  initModel();

  m_calc = context()->services->calculatorService();

  setSearchPlaceholderText("Search past calculations...");

  m_calculatorDebounce.setInterval(200ms);
  m_calculatorDebounce.setSingleShot(true);

  connect(&m_calculatorDebounce, &QTimer::timeout, this, &CalcHistoryViewHost::startCalculator);
  connect(&m_calcWatcher, &CalculatorWatcher::finished, this, &CalcHistoryViewHost::handleCalculatorFinished);

  connect(m_calc, &CalculatorService::recordPinned, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::recordUnpinned, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::recordRemoved, this, &CalcHistoryViewHost::refresh);
  connect(m_calc, &CalculatorService::allRecordsRemoved, this, &CalcHistoryViewHost::refresh);

  model()->addSource(&m_liveSection);
}

void CalcHistoryViewHost::loadInitialData() { textChanged(searchText()); }

void CalcHistoryViewHost::textChanged(const QString &text) {
  m_query = text;

  m_calculatorDebounce.stop();
  m_liveSection.clear();

  auto data = m_calc->groupRecordsByTime(m_calc->query(text));
  applyGroupedData(std::move(data));

  if (!text.isEmpty()) { m_calculatorDebounce.start(); }
}

void CalcHistoryViewHost::refresh() { textChanged(m_query); }

void CalcHistoryViewHost::applyGroupedData(CalculatorService::GroupedRecordList data) {
  model()->clearSources();
  m_sections.clear();
  m_sections.reserve(data.size());

  model()->addSource(&m_liveSection);

  for (auto &[name, records] : data) {
    if (records.empty()) continue;
    auto section = std::make_unique<CalcHistorySection>();
    section->setRecords(name, std::move(records));
    model()->addSource(section.get());
    m_sections.emplace_back(std::move(section));
  }

  model()->rebuild();
}

void CalcHistoryViewHost::startCalculator() {
  if (m_calcWatcher.isRunning()) {
    m_calc->backend()->abort();
    m_calcWatcher.waitForFinished();
  }

  if (!m_calc->backend() || m_query.isEmpty()) return;

  m_calcWatcher.setFuture(m_calc->backend()->asyncCompute(m_query));
}

void CalcHistoryViewHost::handleCalculatorFinished() {
  if (!m_calcWatcher.isFinished()) return;
  auto res = m_calcWatcher.result();
  if (!res) return;

  m_liveSection.setResult(std::move(res.value()));
}
