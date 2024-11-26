#pragma once
#include "command-object.hpp"
#include "commands/calculator-history/calculator-history.hpp"
#include "omnicast.hpp"
#include <QKeyEvent>
#include <QString>
#include <memory>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdir.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlist.h>
#include <qlogging.h>
#include <qwidget.h>

using WidgetFactory = std::function<CommandObject *()>;

class Command : public IActionnable {
public:
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
  WidgetFactory widgetFactory;

  struct ExecuteCommand : public IAction {
    const Command &ref;

    QString name() const override { return "Open command"; }

    ExecuteCommand(const Command &ref) : ref(ref) {}
  };

  ActionList generateActions() const override {
    return {std::make_shared<ExecuteCommand>(*this)};
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
    commands.push_back(
        Command("Calculator history", "pcbcalculator", "Calculator", false,
                "search calculator history",
                []() { return new CalculatorHistoryCommand(); }));
  };
};
