#pragma once
#include "app.hpp"
#include "calculator-database.hpp"
#include "ui/grid-view.hpp"
#include "ui/action_popover.hpp"
#include "ui/list-view.hpp"
#include "ui/virtual-grid.hpp"
#include <qnamespace.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qthreadpool.h>
#include <qtmetamacros.h>

class RemoveCalculatorHistoryEntryAction : public AbstractAction {
  uint id;

public:
  RemoveCalculatorHistoryEntryAction(uint id)
      : AbstractAction("Remove entry", ThemeIconModel{.iconName = ":icons/trash.svg"}), id(id) {}

  void execute(AppWindow &app) override {
    bool removed = app.calculatorDatabase->removeById(id);

    if (removed) {
      app.statusBar->setToast("Entry removed");
    } else {
      app.statusBar->setToast("Failed to remove entry");
    }
  }
};

class CalculatorHistoryListItem : public SimpleListGridItem {
  Q_OBJECT

  CalculatorEntry entry;

public:
  // size_t id() const override { return qHash(QString("history-entry-%1").arg(entry.id)); };

  QList<AbstractAction *> createActions() const override {
    auto removeAction = new RemoveCalculatorHistoryEntryAction(entry.id);

    connect(removeAction, &AbstractAction::didExecute, this, &CalculatorHistoryListItem::removed);

    return {
        new CopyTextAction("Copy result", entry.result),
        new CopyTextAction("Copy expression", entry.expression),
        removeAction,
    };
  }

  CalculatorHistoryListItem(const CalculatorEntry &entry)
      : SimpleListGridItem(":icons/calculator", entry.expression, "", entry.result), entry(entry) {}

signals:
  void removed();
};

class CalculatorHistoryView : public GridView {
  Service<CalculatorDatabase> calculatorDb;
  QString query;

  void reloadSearch(const QString &s) {

    /*
if (s.size() > 1) {
  model->beginSection("Calculator");
  te_parser parser;
  double result = parser.evaluate(s.toLatin1().data());

  if (!std::isnan(result)) {
    auto data = CalculatorItem{.expression = s, .result = result};
    auto item = std::make_shared<BaseCalculatorListItem>(data);

    model->addItem(item);
  }
}
    */

    grid->clearContents();

    auto history = grid->section("History");

    for (const auto &entry : calculatorDb.listAll()) {
      if (!entry.expression.contains(s, Qt::CaseInsensitive) &&
          !entry.result.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      auto item = new CalculatorHistoryListItem(entry);

      connect(item, &CalculatorHistoryListItem::removed, this, [this, history, item]() {
        history->removeItem(item);
        grid->calculateLayout();
      });

      history->addItem(item);
    }

    grid->calculateLayout();
    grid->selectFirst();
  }

  void onSearchChanged(const QString &s) override {
    query = s;
    reloadSearch(s);
  }

  void onMount() override {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
  }

public:
  CalculatorHistoryView(AppWindow &app) : GridView(app), calculatorDb(service<CalculatorDatabase>()) {
    grid->setMargins(10, 5, 10, 5);
  }
};
