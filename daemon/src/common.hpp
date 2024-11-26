#pragma once
#include <QString>
#include <qwidget.h>

class ActionExecutionContext;

class IAction {
public:
  virtual QString name() const = 0;
  virtual QIcon icon() const {
    return QIcon::fromTheme("application-x-executable");
  }
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  virtual ActionList generateActions() const = 0;
};
