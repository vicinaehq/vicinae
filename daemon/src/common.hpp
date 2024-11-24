#pragma once
#include <QString>

class IAction {
public:
  virtual QString name() const = 0;
  virtual void exec(const QList<QString> cmd) const = 0;
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  virtual ActionList generateActions() const = 0;
};
