#pragma once

#include "calculator-database.hpp"
#include "command-object.hpp"
#include "common.hpp"
#include "tinyexpr.hpp"
#include "ui/managed_list.hpp"

class CalculatorHistoryCommand : public CommandObject {
  std::shared_ptr<CalculatorDatabase> cdb;
  ManagedList *list;
  QList<CalculatorEntry> entries;
  QString initText;

  struct Calculator : public IActionnable {
    QString expression;
    QString result;

    struct CopyAction : IAction {
      const Calculator &ref;

      CopyAction(const Calculator &r) : ref(r) {}

      QString name() const override { return "Copy result"; }
      QIcon icon() const override { return QIcon::fromTheme("pcbcalculator"); }
    };

    struct OpenCalculatorHistory : IAction {
      const Calculator &ref;

      OpenCalculatorHistory(const Calculator &r) : ref(r) {}

      QString name() const override { return "Open in calculator history"; }
      QIcon icon() const override { return QIcon::fromTheme("pcbcalculator"); }
    };

    ActionList generateActions() const override {
      return {std::make_shared<CopyAction>(*this),
              std::make_shared<OpenCalculatorHistory>(*this)};
    }

    Calculator(const QString &expression, const QString &result)
        : expression(expression), result(result) {}
  };

public:
  class Factory : public ICommandFactory {
    QString initText;

    virtual CommandObject *operator()(AppWindow *app) {
      return new CalculatorHistoryCommand(app, initText);
    }

  public:
    Factory(const QString &initText) : initText(initText) {}
  };

  CalculatorHistoryCommand(AppWindow *app, const QString &initText = "")
      : CommandObject(app), initText(initText), list(new ManagedList()) {
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 10, 0, 0);
    layout->addWidget(list);

    forwardInputEvents(list);

    widget->setLayout(layout);

    connect(list, &ManagedList::itemActivated,
            [this]() { setToast("Copied in clipboard"); });

    cdb = service<CalculatorDatabase>();
    entries = cdb->list();
  }

  void onAttach() override {
    setSearchPlaceholder("Browse history and do maths...");
    setSearch(initText);
    onSearchChanged(initText);
  }

  QString name() override { return "Calculator history"; }

  QIcon icon() override { return QIcon::fromTheme("pcbcalculator"); }

  void onSearchChanged(const QString &q) override {
    list->clear();

    std::string_view query(q.toLatin1().data());

    if (query.size() > 1) {
      te_parser parser;

      if (double result = parser.evaluate(query); !std::isnan(result)) {
        list->addSection("Calculator");

        auto exprLabel = new QLabel(q);

        exprLabel->setProperty("class", "transform-left");

        auto answerLabel = new QLabel(QString::number(result));
        answerLabel->setProperty("class", "transform-left");

        auto left = new VStack(exprLabel, new Chip("Expression"));
        auto right = new VStack(answerLabel, new Chip("Answer"));

        list->addWidgetItem(new Calculator(q, answerLabel->text()),
                            new TransformResult(left, right));
      }
    }

    QList<CalculatorEntry> matches;

    for (const auto &row : entries) {
      if (row.expression.contains(q) || row.result.contains(q)) {
        matches.push_back(row);
      }
    }

    if (!matches.isEmpty())
      list->addSection("History");

    for (const auto &row : matches) {
      auto format = QString("%1 = %2").arg(row.expression).arg(row.result);

      list->addWidgetItem(new Calculator(row.expression, row.result),
                          new GenericListItem(QIcon::fromTheme("pcbcalculator"),
                                              format, "", "Calculator"));
    }

    list->selectFirstEligible();
  }
};
