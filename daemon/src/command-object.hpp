#pragma once
#include "common.hpp"
#include "omnicast.hpp"
#include "ui/toast.hpp"
#include <qicon.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class ExecutionContext;
class IAction;
class ICommandFactory;

class CommandObject : public QObject {
  Q_OBJECT

  QList<QWidget *> inputFwdTo;
  bool eventFilter(QObject *obj, QEvent *event) override;
  AppWindow *app_ = nullptr;

protected:
  AppWindow *app() const { return app_; }

  void createCompletion(const QList<QString> &inputs, const QString &icon);
  void destroyCompletion();
  QLineEdit *searchbar();
  void setSearchPlaceholder(const QString &s);
  void forwardInputEvents(QWidget *widget);
  void setActions(const QList<std::shared_ptr<IAction>> &actions);

  // t Unless you are dynamically creating objects that will be forwarded input
  //  events you don't need to call this.

  void unforwardInputEvents(QWidget *widget);
  void clearSearch();
  void hideSearch();
  void setSearch(const QString &s);
  void hideWindow() { app()->hide(); }

  void popCurrent() { app()->popCommandObject(); }
  void pushCommand(std::shared_ptr<ICommandFactory> fac) {
    app()->pushCommandObject(fac);
  }

  template <typename T> std::shared_ptr<T> service() {
    return app()->service<T>();
  }

public:
  friend ExecutionContext;
  QWidget *widget;

  CommandObject(AppWindow *app);
  virtual ~CommandObject();

public slots:
  virtual void onSearchChanged(const QString &) {}
  virtual void onActionActivated(std::shared_ptr<IAction> action) {}
  virtual void onAttach();
  virtual void onDetach();
  virtual void onMount();

  void setToast(const QString &message,
                ToastPriority priority = ToastPriority::Success);
  QString query() const;
  QList<QString> completions() const;

  virtual QString name();
  virtual QIcon icon();
};

class ExecutionContext {
  CommandObject &obj;

public:
  ExecutionContext(CommandObject &obj) : obj(obj) {}

  void setToast(const QString &msg, ToastPriority priority) {
    obj.setToast(msg, priority);
  }

  QList<QString> completions() { return obj.completions(); }

  QString query() const { return obj.query(); }

  void setSearch(const QString &s) { obj.setSearch(s); }

  void hideWindow() { obj.hideWindow(); }

  void reloadSearch() { obj.onSearchChanged(obj.query()); }

  void pushCommand(std::shared_ptr<ICommandFactory> fac) {
    return obj.pushCommand(fac);
  }

  void popCurrent() { return obj.popCurrent(); }

  template <typename T> Service<T> service() { return obj.service<T>(); }
};

class IAction {

public:
  virtual QString name() const = 0;
  virtual QIcon icon() const {
    return QIcon::fromTheme("application-x-executable");
  }

  virtual void exec(ExecutionContext ctx) = 0;

  IAction() {}
};

class IActionnable {
protected:
  using ActionList = QList<std::shared_ptr<IAction>>;

public:
  IActionnable() {}
  virtual ActionList generateActions() const = 0;
};
