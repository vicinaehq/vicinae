#pragma once
#include "common.hpp"
#include "omnicast.hpp"
#include <qobject.h>
#include <qwidget.h>

class CommandObject : public QObject {
  Q_OBJECT

  QList<QWidget *> inputFwdTo;
  bool eventFilter(QObject *obj, QEvent *event) override;

protected:
  AppWindow *app = nullptr;

  void createCompletion(const QList<QString> &inputs, const QString &icon);
  void destroyCompletion();
  QLineEdit *searchbar();
  void setSearchPlaceholder(const QString &s);
  void forwardInputEvents(QWidget *widget);
  void setToast(const QString &message,
                ToastPriority priority = ToastPriority::Success);
  void setActions(const QList<std::shared_ptr<IAction>> &actions);

  QString query() const;
  QList<QString> completions() const;

  QList<QString> parseCommandLine();

  // Unless you are dynamically creating objects that will be forwarded input
  // events you don't need to call this.

  void unforwardInputEvents(QWidget *widget);
  void clearSearch();
  void setSearch(const QString &s);

public:
  QWidget *widget;

  CommandObject(AppWindow *app);
  virtual ~CommandObject();

public slots:
  virtual void onSearchChanged(const QString &) {}
  virtual void onActionActivated(std::shared_ptr<IAction> action) {}
};
