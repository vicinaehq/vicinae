#pragma once
#include "app.hpp"
#include "calculator-database.hpp"
#include "navigation-list-view.hpp"
#include "ui/action_popover.hpp"
#include "ui/calculator-list-item-widget.hpp"
#include <qnamespace.h>
#include <qthreadpool.h>

class CalculatorHistoryView : public NavigationListView {
  QList<CalculatorEntry> entries;
  Service<CalculatorDatabase> calculatorDb;

  class CalculatorHistoryListItem : public AbstractNativeListItem {
    CalculatorEntry entry;

  public:
    QWidget *createItem() const override {
      return new ListItemWidget(
          ImageViewer::createFromModel(ThemeIconModel{.iconName = ":icons/calculator"}, {25, 25}),
          entry.expression, "", entry.result);
    }

    QList<AbstractAction *> createActions() const override {
      return {new CopyTextAction("Copy result", entry.result),
              new CopyTextAction("Copy expression", entry.expression)};
    }

    CalculatorHistoryListItem(const CalculatorEntry &entry) : entry(entry) {}
  };

  class CalculatorListItem : public AbstractNativeListItem {
    CalculatorItem item;

    QWidget *createItem() const override { return new CalculatorListItemWidget(item); }

    QList<AbstractAction *> createActions() const override {
      QString sresult = QString::number(item.result);

      return {
          new CopyCalculatorResultAction(item, "Copy result", sresult),
          new CopyCalculatorResultAction(item, "Copy expression",
                                         QString("%1 = %2").arg(item.expression).arg(sresult)),
      };
    }

  public:
    CalculatorListItem(const CalculatorItem &item) : item(item) {}
  };

  void onSearchChanged(const QString &s) override {
    model->beginReset();

    if (s.size() > 1) {
      model->beginSection("Calculator");
      Parser parser;

      if (auto result = parser.evaluate(s.toLatin1().data())) {
        auto value = result.value();
        auto data = CalculatorItem{.expression = s, .result = value.value, .unit = value.unit};
        auto item = std::make_shared<CalculatorListItem>(data);

        model->addItem(item);
      }
    }

    model->beginSection("History");

    for (const auto &entry : entries) {
      if (!entry.expression.contains(s, Qt::CaseInsensitive) &&
          !entry.result.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      model->addItem(std::make_shared<CalculatorHistoryListItem>(entry));
    }
    model->endReset();
  }

  void onMount() override {
    setSearchPlaceholderText("Do maths, convert units or search past calculations...");
  }

public:
  CalculatorHistoryView(AppWindow &app)
      : NavigationListView(app), calculatorDb(service<CalculatorDatabase>()) {
    entries = calculatorDb.list();
  }
};
