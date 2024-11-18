#pragma once
#include "omnicast.hpp"
#include <QString>
#include <memory>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlist.h>
#include <qwidget.h>

class CalculatorHistoryCommand : public CommandWidget {
public:
  CalculatorHistoryCommand(AppWindow *app) : CommandWidget(app) {
    auto layout = new QVBoxLayout();

    layout->addWidget(new QLabel("Calculator history"));

    setLayout(layout);
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
