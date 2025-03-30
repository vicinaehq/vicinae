#pragma once
#include <qobject.h>
#include <qdebug.h>

class AbstractCommand;

class AppWindow;
class View;

class CommandContext : public QObject {
  AppWindow *_app;
  std::shared_ptr<AbstractCommand> _cmd;

public:
  AppWindow *app() const { return _app; }
  const AbstractCommand *command() const { return _cmd.get(); }

  virtual void load() {}
  virtual void unload() {};

  CommandContext(AppWindow *app, const std::shared_ptr<AbstractCommand> &command) : _app(app) {}
};

class ViewCommandContext : public CommandContext {
public:
  ViewCommandContext(AppWindow *app, const std::shared_ptr<AbstractCommand> &command)
      : CommandContext(app, command) {}

  virtual View *view() const = 0;

  ~ViewCommandContext() { qDebug() << "destroyed view"; }
};

class HeadlessCommand : public CommandContext {
  virtual void load() = 0;
};

template <typename T> class SingleViewCommand : public ViewCommandContext {
public:
  SingleViewCommand(AppWindow *app, const std::shared_ptr<AbstractCommand> &command)
      : ViewCommandContext(app, command) {}
  View *view() const override { return new T(*app()); }
};
