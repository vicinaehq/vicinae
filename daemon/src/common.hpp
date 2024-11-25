#pragma once
#include <QString>
#include <qwidget.h>

class IAction {
public:
  virtual QString name() const = 0;
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  virtual ActionList generateActions() const = 0;
};

template <class T> class Clonable : public T {
  virtual T *clone() = 0;
};
