#pragma once
#include "calculator-database.hpp"
#include "index-command.hpp"
#include "omnicast.hpp"
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
  ManagedList *list;
  QList<CalculatorEntry> entries;

public:
  CalculatorHistoryCommand(AppWindow *app)
      : CommandWidget(app), list(new ManagedList()) {
    auto layout = new QVBoxLayout();

    for (const auto &row : CalculatorDatabase::get().list()) {
      qDebug() << row.expression << " = " << row.result << " at "
               << row.timestamp;
    }

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);

    connect(searchbar(), &QLineEdit::textChanged, this,
            &CalculatorHistoryCommand::textChanged);

    searchbar()->clear();
    searchbar()->installEventFilter(this);
    searchbar()->setPlaceholderText("Browse history and do maths");

    entries = CalculatorDatabase::get().list();
    textChanged("");

    setLayout(layout);
  }

  bool isListKey(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
      return true;
    default:
      return false;
    }
  }

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      qDebug() << QKeySequence(keyEvent->key()).toString();

      if (keyEvent->key() == Qt::Key_Return && app->topBar->quickInput) {
        if (app->topBar->quickInput->focusFirstEmpty())
          return true;
      }

      if (isListKey(keyEvent)) {
        QApplication::sendEvent(list, event);
        return true;
      }
    }

    return false;
  }

public slots:
  void textChanged(const QString &q) {
    list->clear();
    list->addSection("History");

    for (const auto &row : entries) {
      bool matches = row.expression.contains(q) || row.result.contains(q);

      if (!matches)
        continue;

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
