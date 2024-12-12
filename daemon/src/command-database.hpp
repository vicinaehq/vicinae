#pragma once
#include "command-object.hpp"
#include "common.hpp"
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

class CommandInfo {
public:
  QString name;
  QString iconName;
  QString category;
  bool usableWith;
  QString normalizedName;
  std::shared_ptr<ICommandFactory> widgetFactory;

  CommandInfo(const QString &name, const QString &iconName,
              const QString &category, bool usableWith,
              const QString &normalizedName,
              std::shared_ptr<ICommandFactory> factory)
      : name(name), iconName(iconName), category(category),
        usableWith(usableWith), normalizedName(normalizedName),
        widgetFactory(factory) {}
};

class ActionnableCommand : public IActionnable {
public:
  struct ExecuteCommand : public IAction {
    CommandInfo cmd;

    QString name() const override { return "Open command"; }
    QIcon icon() const override { return QIcon::fromTheme(""); }
    void exec(ExecutionContext ctx) override {
      ctx.pushCommand(cmd.widgetFactory);
    }

    ExecuteCommand(const CommandInfo &cmd) : cmd(cmd) {}
  };

  CommandInfo cmd;

  ActionnableCommand(ExecutionContext ctx, const CommandInfo &cmd) : cmd(cmd) {}

  ActionList generateActions() const override {
    return {std::make_shared<ExecuteCommand>(cmd)};
  }
};

struct CommandDatabase {
  QList<CommandInfo> commands;

  CommandDatabase();
};
