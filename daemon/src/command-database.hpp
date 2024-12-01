#pragma once
#include "command-object.hpp"
#include "commands/calculator-history/calculator-history.hpp"
#include "commands/create-quicklink/create-quicklink.hpp"
#include "commands/manage-quicklinks/quicklink-manager.hpp"
#include "common.hpp"
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

class Command {
public:
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
  std::shared_ptr<ICommandFactory> widgetFactory;

  Command(const QString &name, const QString &iconName, const QString &category,
          bool usableWith, const QString &normalizedName,
          std::shared_ptr<ICommandFactory> factory)
      : name(name), iconName(iconName), category(category),
        usableWith(usableWith), normalizedName(normalizedName),
        widgetFactory(factory) {}
};

class ActionnableCommand : public IActionnable {
public:
  struct ExecuteCommand : public IAction {
    Command cmd;

    QString name() const override { return "Open command"; }
    QIcon icon() const override { return QIcon::fromTheme(""); }
    void exec(ExecutionContext ctx) override {
      ctx.pushCommand(cmd.widgetFactory);
    }

    ExecuteCommand(const Command &cmd) : cmd(cmd) {}
  };

  Command cmd;

  ActionnableCommand(ExecutionContext ctx, const Command &cmd) : cmd(cmd) {}

  ActionList generateActions() const override {
    return {std::make_shared<ExecuteCommand>(cmd)};
  }
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
        std::make_shared<BasicCommandFactory<CalculatorHistoryCommand>>()));
    commands.push_back(Command(
        "Browse quicklinks", "link", "Quicklink", false, "browse quicklink",
        std::make_shared<BasicCommandFactory<QuickLinkManagerCommand>>()));

    commands.push_back(Command(
        "Create quicklink", "link", "Quicklink", false, "create quicklink",
        std::make_shared<BasicCommandFactory<CreateQuickLinkCommand>>()));
  };
};
