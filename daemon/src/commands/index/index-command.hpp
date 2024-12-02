#pragma once
#include "command-object.hpp"
#include "ui/managed_list.hpp"

struct CodeToColor : public IActionnable {
  QString input;

  ActionList generateActions() const override { return {}; }

  CodeToColor(ExecutionContext ctx, const QString &input) : input(input) {}
};

class CommandDatabase;
class QuicklistDatabase;
class Command;

class IndexCommand : public CommandObject {
  Q_OBJECT;

  Service<AppDatabase> appDb;
  QString query;
  CommandDatabase *cmdDb;
  QList<Command *> usableWithCommands;

  Service<QuicklistDatabase> quicklinkDb;
  ManagedList *list = nullptr;

private:
  void inputTextChanged(const QString &);
  void itemSelected(const IActionnable &item);
  void itemActivated(const IActionnable &item);

public:
  IndexCommand(AppWindow *app);

  void onAttach() override;
  void onSearchChanged(const QString &) override;
  void onActionActivated(std::shared_ptr<IAction> action) override;
};
