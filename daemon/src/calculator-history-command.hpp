#pragma once
#include "app.hpp"
#include "calculator-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/native-list.hpp"
#include "ui/toast.hpp"
#include "view.hpp"
#include <qnamespace.h>
#include <variant>

struct CalculatorCopyResultAction {
  QString result;
};

using HistoryAction = std::variant<CalculatorCopyResultAction>;

struct HistoryRow {
  QString expression;
  QString result;
};

class CalculatorHistoryView : public View,
                              public TypedNativeListDelegate<CalculatorEntry> {
  AppWindow &app;
  NativeList *list;
  TypedNativeListModel<CalculatorEntry> *model;
  QList<CalculatorEntry> entries;
  Service<ClipboardService> clipboardService;
  Service<CalculatorDatabase> calculatorDb;

  QWidget *createItemFromVariant(const CalculatorEntry &variant) override {
    return new ListItemWidget(
        ImageViewer::createFromModel(
            ThemeIconModel{.iconName = "pcbcalculator"}, {25, 25}),
        variant.result, "", "Entry");
  }

  void copyResult(QString result) {
    qDebug() << "copy result";
    clipboardService.copyText(result);
    app.statusBar->setToast("Copied");
  }

  void copyExpression(QString expression) {
    qDebug() << "copy expression";
    clipboardService.copyText(expression);
    app.statusBar->setToast("Copied");
  }

  void removeEntry(int id) {
    auto removed = calculatorDb.removeById(id);

    if (!removed) {
      app.statusBar->setToast("Failed to remove entry", ToastPriority::Danger);
    } else {
      app.statusBar->setToast("Entry removed");
      model->removeItemIf([id](const QVariant &variant) {
        return id == variant.value<CalculatorEntry>().id;
      });
      entries.removeIf([id](auto &item) { return item.id == id; });
    }
  }

  void variantSelectionChanged(const CalculatorEntry &variant) override {
    ActionData copyResultAction{
        .title = "Copy result",
        .icon = ThemeIconModel{.iconName = "pcbcalculator"},
        .execute = std::bind(&CalculatorHistoryView::copyResult, this,
                             variant.result)};
    ActionData copyExpression{
        .title = "Copy expression",
        .icon = ThemeIconModel{.iconName = "pcbcalculator"},
        .execute = std::bind(&CalculatorHistoryView::copyResult, this,
                             variant.result)};
    ActionData removeEntry{
        .title = "Remove entry",
        .icon = ThemeIconModel{.iconName = "pcbcalculator"},
        .execute =
            std::bind(&CalculatorHistoryView::removeEntry, this, variant.id)};

    setActions({copyResultAction, copyExpression, removeEntry});
  }

  void onSearchChanged(const QString &s) override {
    model->beginReset();
    model->beginSection("History");

    for (const auto &entry : entries) {
      if (!entry.expression.contains(s, Qt::CaseInsensitive) &&
          !entry.result.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      model->addItem(entry);
    }
    model->endReset();
  }

  void onMount() override {
    setSearchPlaceholderText("Browse calculator history...");
  }

public:
  CalculatorHistoryView(AppWindow &app)
      : View(app), app(app), list(new NativeList),
        clipboardService(service<ClipboardService>()),
        calculatorDb(service<CalculatorDatabase>()),
        model(new TypedNativeListModel<CalculatorEntry>()) {
    list->setItemDelegate(this);
    list->setModel(model);

    entries = app.calculatorDatabase->list();
    qDebug() << "listed " << entries.size();
    widget = list;
  }
};
