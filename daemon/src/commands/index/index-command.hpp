#pragma once
#include "command-object.hpp"
#include "omnicast.hpp"
#include "ui/managed_list.hpp"

struct CodeToColor : public IActionnable {
  QString input;

  ActionList generateActions() const override { return {}; }

  CodeToColor(const QString &input) : input(input) {}
};

class CommandDatabase;
class XdgDesktopDatabase;
class QuicklistDatabase;
class Command;

class IndexCommand : public CommandObject {
  Q_OBJECT;

  QString query;
  XdgDesktopDatabase *xdg;
  CommandDatabase *cmdDb;
  QuicklistDatabase *quicklinkDb;
  QList<Command *> usableWithCommands;
  ManagedList *list = nullptr;

  struct Calculator : public IActionnable {
    QString expression;
    QString result;

    struct CopyAction : IAction {
      const Calculator &ref;

      CopyAction(const Calculator &r) : ref(r) {}

      QString name() const override { return "Copy result"; }
      QIcon icon() const override { return QIcon::fromTheme("pcbcalculator"); }
    };

    struct OpenCalculatorHistory : IAction {
      const Calculator &ref;

      OpenCalculatorHistory(const Calculator &r) : ref(r) {}

      QIcon icon() const override { return QIcon::fromTheme("pcbcalculator"); }
      QString name() const override { return "Open calculator historty"; }
    };

    ActionList generateActions() const override {
      return {std::make_shared<CopyAction>(*this),
              std::make_shared<OpenCalculatorHistory>(*this)};
    }

    Calculator(const QString &expression, const QString &result)
        : expression(expression), result(result) {}
  };

private:
  void inputTextChanged(const QString &);
  void itemSelected(const IActionnable &item);
  void itemActivated(const IActionnable &item);

public:
  IndexCommand();

  void onAttach() override;
  void onMount() override;
  void onSearchChanged(const QString &) override;
  void onActionActivated(std::shared_ptr<IAction> action) override;
};
