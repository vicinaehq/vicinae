#include "calc-history-model.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"

void CalcHistorySection::setRecords(const QString &groupName, std::vector<CalculatorRecord> records) {
  m_groupName = groupName;
  m_records = std::move(records);
}

QString CalcHistorySection::itemTitle(int i) const { return m_records[i].question; }

QString CalcHistorySection::itemIconSource(int i) const {
  const auto &record = m_records[i];
  switch (record.typeHint) {
  case AbstractCalculatorBackend::CONVERSION:
    return imageSourceFor(ImageURL::builtin("switch"));
  default:
    return imageSourceFor(ImageURL::builtin("calculator"));
  }
}

QVariantList CalcHistorySection::itemAccessories(int i) const {
  return qml::textAccessory(m_records[i].answer);
}

std::unique_ptr<ActionPanelState> CalcHistorySection::actionPanel(int i) const {
  const auto &record = m_records[i];

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
