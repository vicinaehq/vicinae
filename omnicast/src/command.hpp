#pragma once

#include <qobject.h>
#include <qdebug.h>

class AppWindow;
class View;

class Command : public QObject {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

class HeadlessCommand : public Command {
  virtual void load() = 0;
};

template <typename T> class SingleViewCommand : public ViewCommand {
  View *load(AppWindow &app) override { return new T(app); }

  void unload(AppWindow &) override { qDebug() << "Command unloaded"; }
};
