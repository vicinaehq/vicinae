#include "qml-calc-history-model.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"

QmlCalcHistoryModel::QmlCalcHistoryModel(QObject *parent) : QmlCommandListModel(parent) {}

void QmlCalcHistoryModel::initialize(ApplicationContext *ctx) {
  QmlCommandListModel::initialize(ctx);
  m_calc = ctx->services->calculatorService();

  connect(m_calc, &CalculatorService::recordPinned, this, &QmlCalcHistoryModel::refresh);
  connect(m_calc, &CalculatorService::recordUnpinned, this, &QmlCalcHistoryModel::refresh);
  connect(m_calc, &CalculatorService::recordRemoved, this, &QmlCalcHistoryModel::refresh);
  connect(m_calc, &CalculatorService::allRecordsRemoved, this, &QmlCalcHistoryModel::refresh);
}

void QmlCalcHistoryModel::setFilter(const QString &text) {
  m_query = text;
  m_data = m_calc->groupRecordsByTime(m_calc->query(text));

  std::vector<SectionInfo> sections;
  sections.reserve(m_data.size());
  for (const auto &[name, records] : m_data) {
    sections.push_back({.name = name, .count = static_cast<int>(records.size())});
  }
  setSections(sections);
}

void QmlCalcHistoryModel::refresh() { setFilter(m_query); }

QString QmlCalcHistoryModel::itemTitle(int s, int i) const { return m_data[s].second[i].question; }

QString QmlCalcHistoryModel::itemIconSource(int s, int i) const {
  const auto &record = m_data[s].second[i];
  switch (record.typeHint) {
  case AbstractCalculatorBackend::CONVERSION:
    return imageSourceFor(ImageURL::builtin("switch"));
  default:
    return imageSourceFor(ImageURL::builtin("calculator"));
  }
}

QString QmlCalcHistoryModel::itemAccessory(int s, int i) const { return m_data[s].second[i].answer; }

std::unique_ptr<ActionPanelState> QmlCalcHistoryModel::createActionPanel(int s, int i) const {
  const auto &record = m_data[s].second[i];

  auto panel = std::make_unique<ListActionPanelState>();

  auto *pinSection = panel->createSection();
  if (record.pinnedAt) {
    pinSection->addAction(new UnpinCalculatorHistoryRecordAction(record.id));
  } else {
    pinSection->addAction(new PinCalculatorHistoryRecordAction(record.id));
  }

  auto *copySection = panel->createSection();
  auto *copyAnswer = new CopyToClipboardAction(Clipboard::Text(record.answer), "Copy answer");
  copyAnswer->setPrimary(true);
  copySection->addAction(copyAnswer);
  copySection->addAction(new CopyToClipboardAction(Clipboard::Text(record.question), "Copy question"));
  copySection->addAction(
      new CopyToClipboardAction(Clipboard::Text(record.expression()), "Copy question and answer"));

  auto *dangerSection = panel->createSection();
  dangerSection->addAction(new RemoveCalculatorHistoryRecordAction(record.id));
  dangerSection->addAction(new RemoveAllCalculatorHistoryRecordsAction());

  return panel;
}
