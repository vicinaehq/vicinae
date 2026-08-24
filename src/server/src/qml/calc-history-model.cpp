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

std::optional<ImageURL> CalcHistorySection::itemIcon(int i) const {
  const auto &record = m_records[i];
  switch (record.typeHint) {
  case AbstractCalculatorBackend::CONVERSION:
    return ImageURL::builtin(BuiltinIcon::Switch);
  default:
    return ImageURL::builtin(BuiltinIcon::Calculator);
  }
}

AccessoryList CalcHistorySection::itemAccessories(int i) const { return {{.text = m_records[i].answer}}; }

std::unique_ptr<ActionPanelState> CalcHistorySection::actionPanel(int i) const {
  const auto &record = m_records[i];

  auto panel = std::make_unique<ListActionPanelState>();

  auto *copySection = panel->createSection();
  auto *copyAnswer = new CopyToClipboardAction(Clipboard::Text(record.answer), tr("Copy answer"));
  auto *pasteAnswer = new PasteToFocusedWindowAction(Clipboard::Text(record.answer));
  addCalculatorPasteCopyActions(copySection, pasteAnswer, copyAnswer);
  copySection->addAction(new CopyToClipboardAction(Clipboard::Text(record.question), tr("Copy question")));
  copySection->addAction(
      new CopyToClipboardAction(Clipboard::Text(record.expression()), tr("Copy question and answer")));

  auto *pinSection = panel->createSection();
  if (record.pinnedAt) {
    pinSection->addAction(new UnpinCalculatorHistoryRecordAction(record.id));
  } else {
    pinSection->addAction(new PinCalculatorHistoryRecordAction(record.id));
  }

  auto *dangerSection = panel->createSection();
  dangerSection->addAction(new RemoveCalculatorHistoryRecordAction(record.id));
  dangerSection->addAction(new RemoveAllCalculatorHistoryRecordsAction());

  return panel;
}
