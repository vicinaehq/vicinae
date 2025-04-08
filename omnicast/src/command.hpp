#pragma once
#include "ui/action-pannel/action.hpp"
#include <qobject.h>
#include <qdebug.h>

class AbstractCmd;
class BuiltinCommand;
class AppWindow;
class View;

class CommandContext : public QObject {
  AppWindow *_app;
  std::shared_ptr<AbstractCmd> _cmd;

public:
  AppWindow *app() const { return _app; }
  const AbstractCmd *command() const { return _cmd.get(); }
  virtual void onActionExecuted(AbstractAction *action) {}

  virtual void load() {}
  virtual void unload() {};

  CommandContext(AppWindow *app, const std::shared_ptr<AbstractCmd> &command) : _app(app), _cmd(command) {}
};
