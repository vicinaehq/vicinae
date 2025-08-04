#pragma once
#include "action-panel/action-panel.hpp"
#include "actions/calculator/calculator-actions.hpp"
#include "ui/views/base-view.hpp"
#include "clipboard-actions.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "services/calculator-service/abstract-calculator-backend.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include "ui/omni-list/omni-list.hpp"
#include <qnamespace.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qthreadpool.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include "ui/views/list-view.hpp"

class CalculatorListItem : public OmniList::AbstractVirtualItem, public ListView::Actionnable {
protected:
  const AbstractCalculatorBackend::CalculatorResult item;

  OmniListItemWidget *createWidget() const override {
    return new CalculatorListItemWidget(CalculatorItem{.expression = item.question, .result = item.answer});
  }

  int calculateHeight(int width) const override {
    static CalculatorListItemWidget ruler({});

    return ruler.sizeHint().height();
  }

  QString generateId() const override { return item.question; }

  QList<AbstractAction *> generateActions() const override {
    QString sresult = item.answer;
    auto copyAnswer = new CopyCalculatorAnswerAction(item);
    auto copyQA = new CopyCalculatorQuestionAndAnswerAction(item);
    auto putAnswerInSearchBar = new PutCalculatorAnswerInSearchBar(item);

    copyAnswer->setPrimary(true);

    return {copyAnswer, copyQA, putAnswerInSearchBar};
  }

public:
  CalculatorListItem(const AbstractCalculatorBackend::CalculatorResult &item) : item(item) {}
};

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
  QString generateId() const override { return QString::number(m_record.id); };

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

class CalculatorHistoryView : public ListView {
  QString m_searchQuery;
  CalculatorService *m_calculator;
  QTimer *m_calcDebounce = new QTimer(this);
  std::optional<AbstractCalculatorBackend::CalculatorResult> m_calcRes;

  void handlePinned(int id) { textChanged(m_searchQuery); }

  void handleUnpinned(int id) { textChanged(m_searchQuery); }

  void handleRemoved(int id) { textChanged(m_searchQuery); }

  void handleAllRemoved() { textChanged(m_searchQuery); }

  void generateRootList() {
    m_list->updateModel([&]() {
      for (const auto &[group, records] : m_calculator->groupRecordsByTime(m_calculator->records())) {
        auto &section = m_list->addSection(group);

        for (const auto &record : records) {
          section.addItem(std::make_unique<CalculatorHistoryListItem>(record));
        }
      }
    });
  }

  void generateFilteredList(const QString &text) {
    m_list->updateModel([&]() {
      if (m_calcRes) {
        m_list->addSection("Calculator").addItem(std::make_unique<CalculatorListItem>(*m_calcRes));
      }

      for (const auto &[group, records] : m_calculator->groupRecordsByTime(m_calculator->query(text))) {
        auto &section = m_list->addSection(group);

        for (const auto &record : records) {
          section.addItem(std::make_unique<CalculatorHistoryListItem>(record));
        }
      }
    });
  }

  void handleCalculatorTimeout() {
    QString expression = searchText().trimmed();
    bool isComputable = false;

    for (const auto &ch : expression) {
      if (!ch.isLetterOrNumber() || ch.isSpace()) {
        isComputable = true;
        break;
      }
    }

    if (!isComputable) {
      m_calcRes.reset();
      return;
    }

    auto result = m_calculator->backend()->compute(expression);

    if (result) {
      m_calcRes = *result;
    } else {
      m_calcRes.reset();
    }
    generateFilteredList(m_searchQuery);
  }

  void textChanged(const QString &text) override {
    m_searchQuery = text;

    if (text.isEmpty()) {
      m_calcRes.reset();
      return generateRootList();
    }

    m_calcDebounce->start(100);

    return generateFilteredList(text);
  }

  void initialize() override {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
    generateRootList();
  }

public:
  CalculatorHistoryView() : m_calculator(ServiceRegistry::instance()->calculatorService()) {
    m_calcDebounce->setSingleShot(true);
    connect(m_calculator, &CalculatorService::recordPinned, this, &CalculatorHistoryView::handlePinned);
    connect(m_calculator, &CalculatorService::recordUnpinned, this, &CalculatorHistoryView::handleUnpinned);
    connect(m_calculator, &CalculatorService::recordRemoved, this, &CalculatorHistoryView::handleRemoved);
    connect(m_calculator, &CalculatorService::allRecordsRemoved, this,
            &CalculatorHistoryView::handleAllRemoved);
    connect(m_calcDebounce, &QTimer::timeout, this, &CalculatorHistoryView::handleCalculatorTimeout);
  }
};
