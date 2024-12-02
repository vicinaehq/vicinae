#pragma once

#include "calculator-database.hpp"
#include "command-object.hpp"
#include "common.hpp"
#include "ui/managed_list.hpp"

class CalculatorHistoryCommand : public CommandObject {
  Service<CalculatorDatabase> cdb;
  ManagedList *list;
  QList<CalculatorEntry> entries;
  QString initText;

public:
  class Factory : public ICommandFactory {
    QString initText;

    virtual CommandObject *operator()(AppWindow *app) {
      return new CalculatorHistoryCommand(app, initText);
    }

  public:
    Factory(const QString &initText) : initText(initText) {}
  };

  CalculatorHistoryCommand(AppWindow *app, const QString &initText = "");

  void onAttach() override;
  QString name() override { return "Calculator history"; }
  QIcon icon() override { return QIcon::fromTheme("pcbcalculator"); }
  void onSearchChanged(const QString &q) override;
};
