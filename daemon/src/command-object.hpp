#pragma once
#include "omnicast.hpp"
#include "ui/toast.hpp"
#include <qicon.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class IAction;

class CommandObject : public QObject {
  Q_OBJECT

  QList<QWidget *> inputFwdTo;
  bool eventFilter(QObject *obj, QEvent *event) override;
  AppWindow *app_ = nullptr;

protected:
  AppWindow *app() const { return static_cast<AppWindow *>(parent()); }

  void createCompletion(const QList<QString> &inputs, const QString &icon);
  void destroyCompletion();
  QLineEdit *searchbar();
  void setSearchPlaceholder(const QString &s);
  void forwardInputEvents(QWidget *widget);
  void setActions(const QList<std::shared_ptr<IAction>> &actions);

  QList<QString> parseCommandLine();

  // t Unless you are dynamically creating objects that will be forwarded input
  //  events you don't need to call this.

  void unforwardInputEvents(QWidget *widget);
  void clearSearch();
  void setSearch(const QString &s);
  void pushCommand(CommandObject *next) { app()->pushCommandObject(next); }

public:
  friend ActionExecutionContext;

  QWidget *widget;

  CommandObject();
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

  virtual const QString &name();
  virtual QIcon icon();
};

class ActionExecutionContext {
  CommandObject &obj;

public:
  ActionExecutionContext(CommandObject &obj) : obj(obj) {}

  void setToast(const QString &message,
                ToastPriority priority = ToastPriority::Success) {
    obj.setToast(message, priority);
  }

  void pushCommand(CommandObject *next) { obj.app()->pushCommandObject(next); }
};
