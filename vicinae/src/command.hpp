#pragma once
#include "common.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/toast/toast.hpp"
#include <qobject.h>
#include <qdebug.h>
#include <qtmetamacros.h>

class AbstractCmd;
class BuiltinCommand;
class AppWindow;
class View;
class LaunchProps;

class CommandContext : public QObject {
  Q_OBJECT

  std::shared_ptr<AbstractCmd> _cmd;
  ApplicationContext *m_ctx = nullptr;

public:
  const AbstractCmd *command() const { return _cmd.get(); }
  virtual void onActionExecuted(AbstractAction *action) {}

  void setContext(ApplicationContext *ctx) { m_ctx = ctx; }
  ApplicationContext *context() const { return m_ctx; }

  virtual void load(const LaunchProps &props) {}
  virtual void unload() {};

  CommandContext(const std::shared_ptr<AbstractCmd> &command) : _cmd(command) {}

signals:
  void requestPushView(View *view) const;
  void requestTitleChange(const QString &title) const;
  void requestSearchTextChange(const QString &text) const;
  void requestToast(const QString &text, ToastPriority priority = ToastPriority::Success) const;
  void requestPopView() const;
  void requestPopToRoot() const;
  void requestWindowClose() const;
  void requestUnload() const;
};
