#include "commands/calculator-history/calculator-history.hpp"
#include "actions.hpp"
#include "calculator.hpp"
#include "command-object.hpp"
#include "omnicast.hpp"
#include "ui/managed_list.hpp"

CalculatorHistoryCommand::CalculatorHistoryCommand(AppWindow *app,
                                                   const QString &initText)
    : CommandObject(app), cdb(service<CalculatorDatabase>()),
      initText(initText), list(new ManagedList()) {
  auto layout = new QVBoxLayout();

  layout->setContentsMargins(0, 10, 0, 0);
  layout->addWidget(list);

  forwardInputEvents(list);

  widget->setLayout(layout);

  connect(list, &ManagedList::itemActivated,
          [this]() { setToast("Copied in clipboard"); });

  entries = cdb.list();
}

void CalculatorHistoryCommand::onAttach() {
  setSearchPlaceholder("Browse history and do maths...");
  setSearch(initText);
  onSearchChanged(initText);
}

void CalculatorHistoryCommand::onSearchChanged(const QString &q) {
  ExecutionContext ctx(*this);

  list->clear();

  std::string_view query(q.toLatin1().data());

  if (q.size() > 1) {
    Parser parser;

    if (auto result = parser.evaluate(query)) {
      auto value = result.value();
      list->addSection("Calculator");

      auto exprLabel = new QLabel(q);

      exprLabel->setProperty("class", "transform-left");

      auto answerLabel = new QLabel(QString::number(value.value));
      answerLabel->setProperty("class", "transform-left");

      auto left = new VStack(exprLabel, new Chip("Expression"));
      auto right = new VStack(
          answerLabel,
          new Chip(value.unit ? QString(value.unit->displayName.data())
                              : "Answer"));

      list->addWidgetItem(
          new ActionnableCalculator(ctx, q, answerLabel->text()),
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
        new ActionnableCalculator(ctx, row.expression, row.result),
        new GenericListItem(QIcon::fromTheme("pcbcalculator"), format,
                            row.timestamp.toString(), "Calculator"));
  }

  list->selectFirstEligible();
}
