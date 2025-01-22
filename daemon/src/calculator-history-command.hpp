#pragma once
#include "app.hpp"
#include "calculator-database.hpp"
#include "navigation-list-view.hpp"
#include "ui/action_popover.hpp"
#include <qnamespace.h>
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

class CalculatorHistoryListItem : public AbstractNativeListItem {
  Q_OBJECT

  CalculatorEntry entry;

public:
  QWidget *createItem() const override {
    return new ListItemWidget(
        ImageViewer::createFromModel(ThemeIconModel{.iconName = ":icons/calculator"}, {25, 25}),
        entry.expression, "", entry.result);
  }

  QList<AbstractAction *> createActions() const override {
    auto removeAction = new RemoveCalculatorHistoryEntryAction(entry.id);

    connect(removeAction, &AbstractAction::didExecute, this, &CalculatorHistoryListItem::removed);

    return {
        new CopyTextAction("Copy result", entry.result),
        new CopyTextAction("Copy expression", entry.expression),
        removeAction,
    };
  }

  CalculatorHistoryListItem(const CalculatorEntry &entry) : entry(entry) {}

signals:
  void removed();
};

class CalculatorHistoryView : public NavigationListView {
  Service<CalculatorDatabase> calculatorDb;

  QString query;

  void handleListEntryRemoval() {
    auto oldSelection = list->selected();

    reloadSearch(query);
    list->selectFrom(oldSelection);
  }

  void reloadSearch(const QString &s) {
    model->beginReset();

    if (s.size() > 1) {
      model->beginSection("Calculator");
      Parser parser;

      /*
  if (auto result = parser.evaluate(s.toLatin1().data())) {
    auto value = result.value();
    auto data = CalculatorItem{.expression = s, .result = value.value, .unit = value.unit};
    auto item = std::make_shared<CalculatorListItem>(data);

    model->addItem(item);
  }
      */
    }

    model->beginSection("History");

    for (const auto &entry : calculatorDb.listAll()) {
      if (!entry.expression.contains(s, Qt::CaseInsensitive) &&
          !entry.result.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      auto item = std::make_shared<CalculatorHistoryListItem>(entry);

      connect(item.get(), &CalculatorHistoryListItem::removed, this, [this]() { handleListEntryRemoval(); });

      model->addItem(item);
    }
    model->endReset();
  }

  void onSearchChanged(const QString &s) override {
    query = s;
    reloadSearch(s);
  }

  void onMount() override {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
  }

public:
  CalculatorHistoryView(AppWindow &app)
      : NavigationListView(app), calculatorDb(service<CalculatorDatabase>()) {}
};
