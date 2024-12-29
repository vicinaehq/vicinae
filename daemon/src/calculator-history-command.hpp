

#include "app.hpp"
#include "calculator-database.hpp"
#include "ui/action_popover.hpp"
#include "ui/native-list.hpp"
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
                              public TypedNativeListDelegate<HistoryRow> {
  AppWindow &app;
  NativeList *list;
  TypedNativeListModel<HistoryRow> *model;
  QList<CalculatorEntry> entries;
  Service<ClipboardService> clipboardService;

  QWidget *createItemFromVariant(const HistoryRow &variant) override {
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

  void variantSelectionChanged(const HistoryRow &variant) override {
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

    setActions({copyResultAction, copyExpression});
  }

  void onSearchChanged(const QString &s) override {
    model->beginReset();
    model->beginSection("History");

    for (const auto &entry : entries) {
      if (!entry.expression.contains(s, Qt::CaseInsensitive) &&
          !entry.result.contains(s, Qt::CaseInsensitive)) {
        continue;
      }

      model->addItem(HistoryRow{.result = entry.result});
    }
    model->endReset();
  }

  void onMount() override {}

public:
  CalculatorHistoryView(AppWindow &app)
      : View(app), app(app), list(new NativeList),
        clipboardService(service<ClipboardService>()),
        model(new TypedNativeListModel<HistoryRow>()) {
    list->setItemDelegate(this);
    list->setModel(model);

    entries = app.calculatorDatabase->list();
    qDebug() << "listed " << entries.size();
    widget = list;
  }
};
