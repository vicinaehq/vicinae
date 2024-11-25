#pragma once
#include "calculator-database.hpp"
#include "index-command.hpp"
#include "omnicast.hpp"
#include "tinyexpr.hpp"
#include <QKeyEvent>
#include <QString>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlogging.h>
#include <qwidget.h>

class CalculatorHistoryCommand : public CommandWidget {
  CalculatorDatabase &cdb;
  ManagedList *list;
  QList<CalculatorEntry> entries;

public:
  CalculatorHistoryCommand(AppWindow *app)
      : CommandWidget(app), cdb(CalculatorDatabase::get()),
        list(new ManagedList()) {
    auto layout = new QVBoxLayout();

    for (const auto &row : CalculatorDatabase::get().list()) {
      qDebug() << row.expression << " = " << row.result << " at "
               << row.timestamp;
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);

    setSearchPlaceholder("Browse history and do maths...");
    clearSearch();
    forwardInputEvents(list);

    entries = cdb.list();
    onSearchChanged("");

    setLayout(layout);

    connect(list, &ManagedList::itemActivated,
            [this]() { setToast("Copied in clipboard"); });
  }

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

using WidgetFactory = std::function<CommandWidget *(AppWindow *)>;

class Command : public IActionnable {
public:
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
  WidgetFactory widgetFactory;

  struct ExecuteCommand : public IAction {
    QString name() const override { return "Open command"; }
    void exec(const QList<QString> cmd) const override {
      qDebug() << "execute command";
    }

    ExecuteCommand() {}
  };

  ActionList generateActions() const override {
    return {std::make_shared<ExecuteCommand>()};
  }

  Command(const QString &name, const QString &iconName, const QString &category,
          bool usableWith, const QString &normalizedName, WidgetFactory factory)
      : name(name), iconName(iconName), category(category),
        usableWith(usableWith), normalizedName(normalizedName),
        widgetFactory(factory) {}
};

struct CommandDatabase {
  QList<Command> commands;

  CommandDatabase() {
    /*
commands.push_back(
  Command("Search Files", "search", "File Search", true, "search files"));
          */
    commands.push_back(Command(
        "Calculator history", "pcbcalculator", "Calculator", false,
        "search calculator history",
        [](AppWindow *app) { return new CalculatorHistoryCommand(app); }));
  };
};
