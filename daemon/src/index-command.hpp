#pragma once
#include "calculator-database.hpp"
#include "command-object.hpp"
#include "omnicast.hpp"
#include "ui/managed_list.hpp"

struct CodeToColor : public IActionnable {
  QString input;

  ActionList generateActions() const override { return {}; }

  CodeToColor(const QString &input) : input(input) {}
};

struct Calculator : public IActionnable {
  QString expression;
  QString result;

  struct CopyAction : IAction {
    const Calculator &ref;

    CopyAction(const Calculator &r) : ref(r) {}

    void exec(const QList<QString> cmd) const {
      qDebug() << "copying " << ref.result << " into clipboard";
      CalculatorDatabase::get().saveComputation(ref.expression, ref.result);
    }

    QString name() const override { return "Copy result"; }
  };

  ActionList generateActions() const override {
    return {std::make_shared<CopyAction>(*this)};
  }

  Calculator(const QString &expression, const QString &result)
      : expression(expression), result(result) {}
};

class CommandDatabase;
class XdgDesktopDatabase;
class QuicklistDatabase;
class Command;

class IndexCommand : public CommandObject {
  Q_OBJECT;

  XdgDesktopDatabase *xdg;
  CommandDatabase *cmdDb;
  QuicklistDatabase *quicklinkDb;
  QList<Command *> usableWithCommands;
  ManagedList *list = nullptr;

private:
  void inputTextChanged(const QString &);
  void itemSelected(const IActionnable &item);
  void itemActivated(const IActionnable &item);

public:
  friend IAction;

  IndexCommand(AppWindow *app);

  void onSearchChanged(const QString &) override;
  void onActionActivated(std::shared_ptr<IAction> action) override;
};
