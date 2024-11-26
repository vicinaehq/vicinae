#pragma once

#include "calculator-database.hpp"
#include "command-object.hpp"
#include "tinyexpr.hpp"
#include "ui/managed_list.hpp"

class CalculatorHistoryCommand : public CommandObject {
  CalculatorDatabase &cdb;
  ManagedList *list;
  QList<CalculatorEntry> entries;
  QString n = "Calculator history";
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
  CalculatorHistoryCommand(const QString &initText = "")
      : CommandObject(), initText(initText), cdb(CalculatorDatabase::get()),
        list(new ManagedList()) {
    auto layout = new QVBoxLayout();

    for (const auto &row : CalculatorDatabase::get().list()) {
      qDebug() << row.expression << " = " << row.result << " at "
               << row.timestamp;
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);

    forwardInputEvents(list);

    entries = cdb.list();

    widget->setLayout(layout);

    connect(list, &ManagedList::itemActivated,
            [this]() { setToast("Copied in clipboard"); });
  }

  void onMount() override {}

  void onAttach() override {
    setSearchPlaceholder("Browse history and do maths...");
    setSearch(initText);
    onSearchChanged(initText);
  }

  const QString &name() override { return n; }

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

      list->addWidgetItem(
          new Calculator(row.expression, row.result),
          new GenericListItem("pcbcalculator", format, "", "Calculator"));
    }

    list->selectFirstEligible();
  }
};
