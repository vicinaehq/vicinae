#include "calculator-history-view.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "extensions/calculator/history/calculator-history-controller.hpp"
#include "navigation-controller.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "ui/views/typed-list-view.hpp"

std::unique_ptr<ActionPanelState> CalculatorHistoryView::createActionPanel(const ItemType &record) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto copyAnswer = new CopyToClipboardAction(Clipboard::Text(record.answer), "Copy answer");
  auto copyQuestion = new CopyToClipboardAction(Clipboard::Text(record.question), "Copy question");
  auto copyQuestionAndAnswer =
      new CopyToClipboardAction(Clipboard::Text(record.expression()), "Copy question and answer");
  auto remove = new RemoveCalculatorHistoryRecordAction(record.id);
  auto removeAll = new RemoveAllCalculatorHistoryRecordsAction();
  auto pinSection = panel->createSection();

  if (record.pinnedAt) {
    pinSection->addAction(new UnpinCalculatorHistoryRecordAction(record.id));
  } else {
    pinSection->addAction(new PinCalculatorHistoryRecordAction(record.id));
  }

  auto copySection = panel->createSection();

  copyAnswer->setPrimary(true);
  copySection->addAction(copyAnswer);
  copySection->addAction(copyQuestion);
  copySection->addAction(copyQuestionAndAnswer);

  auto dangerSection = panel->createSection();

  dangerSection->addAction(remove);
  dangerSection->addAction(removeAll);

  return panel;
}

void CalculatorHistoryView::textChanged(const QString &text) { m_controller->setFilter(text); }

void CalculatorHistoryView::initialize() {
  TypedListView::initialize();

  m_model = new CalculatorHistoryModel();
  m_controller = new CalculatorHistoryController(context()->services->calculatorService(), m_model, this);
  setModel(m_model);

  setSearchPlaceholderText("Search past calculations...");
  m_controller->setFilter(searchText());
}
