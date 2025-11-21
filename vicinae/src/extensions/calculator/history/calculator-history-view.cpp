#include "calculator-history-view.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "ui/image/url.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"

class CalculatorHistoryListItem : public AbstractDefaultListItem, public ListView::Actionnable {
  CalculatorService::CalculatorRecord m_record;

  ImageURL icon() const {
    switch (m_record.typeHint) {
    case AbstractCalculatorBackend::NORMAL:
      return ImageURL::builtin("calculator");
    case AbstractCalculatorBackend::CONVERSION:
      return ImageURL::builtin("switch");
    default:
      return ImageURL::builtin("calculator");
    }
  }

public:
  QString generateId() const override { return m_record.id; };

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ActionPanelState>();
    auto copyAnswer = new CopyToClipboardAction(Clipboard::Text(m_record.answer), "Copy answer");
    auto copyQuestion = new CopyToClipboardAction(Clipboard::Text(m_record.question), "Copy question");
    auto copyQuestionAndAnswer =
        new CopyToClipboardAction(Clipboard::Text(m_record.expression()), "Copy question and answer");
    auto remove = new RemoveCalculatorHistoryRecordAction(m_record.id);
    auto removeAll = new RemoveAllCalculatorHistoryRecordsAction();
    auto pinSection = panel->createSection();

    if (m_record.pinnedAt) {
      pinSection->addAction(new UnpinCalculatorHistoryRecordAction(m_record.id));
    } else {
      pinSection->addAction(new PinCalculatorHistoryRecordAction(m_record.id));
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

  ItemData data() const override {
    return {
        .iconUrl = icon(),
        .name = m_record.question,
        .accessories = {{.text = m_record.answer}},
    };
  }

  CalculatorHistoryListItem(const CalculatorService::CalculatorRecord &record) : m_record(record) {}
};

CalculatorHistoryView::CalculatorHistoryView()
    : m_calculator(ServiceRegistry::instance()->calculatorService()) {
  m_calcDebounce->setSingleShot(true);
  connect(&m_pendingComputation, &CalculatorWatcher::finished, this,
          &CalculatorHistoryView::handleComputationFinished);
  connect(m_calculator, &CalculatorService::recordPinned, this, &CalculatorHistoryView::handlePinned);
  connect(m_calculator, &CalculatorService::recordUnpinned, this, &CalculatorHistoryView::handleUnpinned);
  connect(m_calculator, &CalculatorService::recordRemoved, this, &CalculatorHistoryView::handleRemoved);
  connect(m_calculator, &CalculatorService::allRecordsRemoved, this,
          &CalculatorHistoryView::handleAllRemoved);
  connect(m_calcDebounce, &QTimer::timeout, this, &CalculatorHistoryView::handleCalculatorTimeout);
}

void CalculatorHistoryView::handlePinned(const QString &id) { textChanged(m_searchQuery); }

void CalculatorHistoryView::handleUnpinned(const QString &id) { textChanged(m_searchQuery); }

void CalculatorHistoryView::handleRemoved(const QString &id) { textChanged(m_searchQuery); }

void CalculatorHistoryView::handleAllRemoved() { textChanged(m_searchQuery); }

void CalculatorHistoryView::generateRootList() {
  m_list->updateModel([&]() {
    for (const auto &[group, records] : m_calculator->groupRecordsByTime(m_calculator->records())) {
      auto &section = m_list->addSection(group);

      for (const auto &record : records) {
        section.addItem(std::make_unique<CalculatorHistoryListItem>(record));
      }
    }
  });
}

void CalculatorHistoryView::generateFilteredList(const QString &text) {
  m_list->updateModel([&]() {
    /*
if (m_calcRes) {
m_list->addSection("Calculator").addItem(std::make_unique<CalculatorListItem>(*m_calcRes));
}
*/

    for (const auto &[group, records] : m_calculator->groupRecordsByTime(m_calculator->query(text))) {
      auto &section = m_list->addSection(group);

      for (const auto &record : records) {
        section.addItem(std::make_unique<CalculatorHistoryListItem>(record));
      }
    }
  });
}

void CalculatorHistoryView::handleCalculatorTimeout() {
  QString expression = searchText().trimmed();
  bool isComputable = false;

  for (const auto &ch : expression) {
    if (!ch.isLetterOrNumber() || ch.isSpace()) {
      isComputable = true;
      break;
    }
  }

  if (!isComputable || !m_calculator->backend()) {
    m_calcRes.reset();
    return;
  }

  m_pendingComputation.setFuture(m_calculator->backend()->asyncCompute(expression));
}

void CalculatorHistoryView::textChanged(const QString &text) {
  m_searchQuery = text;

  if (m_pendingComputation.isRunning()) { m_pendingComputation.cancel(); }
  if (text.isEmpty()) {
    m_calcRes.reset();
    return generateRootList();
  }

  m_calcDebounce->start(100);

  return generateFilteredList(text);
}

void CalculatorHistoryView::initialize() {
  setSearchPlaceholderText("Search past calculations...");
  generateRootList();
}

void CalculatorHistoryView::handleComputationFinished() {
  if (m_pendingComputation.isCanceled()) return;
  auto result = m_pendingComputation.result();

  if (result) {
    m_calcRes = *result;
  } else {
    m_calcRes.reset();
  }
  generateFilteredList(m_searchQuery);
}
