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

class Command : public IActionnable {
public:
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
  std::shared_ptr<ICommandFactory> widgetFactory;

  struct ExecuteCommand : public IAction {
    const Command &ref;

    QString name() const override { return "Open command"; }
    QIcon icon() const override { return QIcon::fromTheme(ref.iconName); }

    ExecuteCommand(const Command &ref) : ref(ref) {}
  };

  ActionList generateActions() const override {
    return {std::make_shared<ExecuteCommand>(*this)};
  }

  Command(const QString &name, const QString &iconName, const QString &category,
          bool usableWith, const QString &normalizedName,
          std::shared_ptr<ICommandFactory> factory)
      : name(name), iconName(iconName), category(category),
        usableWith(usableWith), normalizedName(normalizedName),
        widgetFactory(std::move(factory)) {}
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
