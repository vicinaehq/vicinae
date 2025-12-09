#include "calculator-history-view.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "extensions/calculator/history/calculator-history-controller.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "ui/views/typed-list-view.hpp"
#include "ui/transform-result/transform-result.hpp"
#include <qboxlayout.h>
#include <qevent.h>

CalculatorHistoryView::CalculatorHistoryView() {}

std::unique_ptr<ActionPanelState> CalculatorHistoryView::createActionPanel(const ItemType &record) const {
  auto panel = std::make_unique<ActionPanelState>();
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

QWidget *CalculatorHistoryView::wrapUI(QWidget *content) {
  auto wrapper = new QWidget(this);
  auto layout = new QVBoxLayout(wrapper);

  m_calcResult = new TransformResult();
  m_calcResult->setVisible(false);
  m_calcResult->setFixedHeight(90);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(m_calcResult);
  layout->addWidget(content, 1);

  return wrapper;
}

bool CalculatorHistoryView::inputFilter(QKeyEvent *event) {
  if (m_currentResult && m_calcResult->isVisible()) {
    if (m_calcResultSelected) {
      if (event->key() == Qt::Key_Down) {
        deselectCalculatorResult();
        return true;
      }
      if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        activateCalculatorResult();
        return true;
      }
      if (event->key() == Qt::Key_Up) { return true; }
    } else if (event->key() == Qt::Key_Up) {
      if (!m_list->selectUp()) {
        selectCalculatorResult();
        return true;
      }
      return true;
    }
  }

  return TypedListView::inputFilter(event);
}

void CalculatorHistoryView::textChanged(const QString &text) { m_controller->setFilter(text); }

void CalculatorHistoryView::initialize() {
  TypedListView::initialize();

  m_model = new CalculatorHistoryModel();
  m_controller = new CalculatorHistoryController(context()->services->calculatorService(), m_model, this);
  setModel(m_model);

  setSearchPlaceholderText("Calculate or search history...");
  m_controller->setFilter(searchText());

  connect(m_controller, &CalculatorHistoryController::calculatorResultChanged, this,
          &CalculatorHistoryView::onCalculatorResultChanged);
}

void CalculatorHistoryView::onCalculatorResultChanged(
    std::optional<AbstractCalculatorBackend::CalculatorResult> result) {
  m_currentResult = result;

  if (result) {
    const auto toDp = [](auto &&u) { return u.displayName; };
    m_calcResult->setBase(result->question.text,
                          result->question.unit.transform(toDp).value_or("Expression"));
    m_calcResult->setResult(result->answer.text, result->answer.unit.transform(toDp).value_or("Answer"));
    m_calcResult->setVisible(true);
  } else {
    m_calcResult->setVisible(false);
    if (m_calcResultSelected) { deselectCalculatorResult(); }
  }
}

void CalculatorHistoryView::selectCalculatorResult() {
  m_calcResultSelected = true;
  m_calcResult->selectionChanged(true);
  updateCalculatorResultActions();
}

void CalculatorHistoryView::deselectCalculatorResult() {
  m_calcResultSelected = false;
  m_calcResult->selectionChanged(false);
  m_list->selectFirst();
}

void CalculatorHistoryView::activateCalculatorResult() {
  if (!m_currentResult) return;
  executePrimaryAction();
}

void CalculatorHistoryView::updateCalculatorResultActions() {
  if (!m_currentResult) {
    clearActions();
    return;
  }

  auto panel = std::make_unique<ActionPanelState>();
  auto copyAnswer = new CopyCalculatorAnswerAction(*m_currentResult, true);
  auto copyQA = new CopyCalculatorQuestionAndAnswerAction(*m_currentResult, true);
  auto putAnswerInSearchBar = new PutCalculatorAnswerInSearchBar(*m_currentResult);
  auto main = panel->createSection();

  copyAnswer->setPrimary(true);
  main->addAction(copyAnswer);
  main->addAction(copyQA);
  main->addAction(putAnswerInSearchBar);

  setActions(std::move(panel));
}
