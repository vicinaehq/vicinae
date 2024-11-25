#pragma once
#include "app.hpp"
#include "ui/status_bar.hpp"
#include <QLineEdit>

class CommandWidget : public QWidget {
  Q_OBJECT

  QList<QWidget *> inputFwdTo;

protected:
  AppWindow *app = nullptr;

  bool eventFilter(QObject *obj, QEvent *event);

  void createCompletion(const QList<QString> &inputs);
  void destroyCompletion();
  QLineEdit *searchbar();
  void setSearchPlaceholder(const QString &s);
  void forwardInputEvents(QWidget *widget);
  void setToast(const QString &message,
                ToastPriority priority = ToastPriority::Success);

  // Unless you are dynamically creating objects that will be forwarded input
  // events you don't need to call this.
  void unforwardInputEvents(QWidget *widget);
  void clearSearch();
  void setSearch(const QString &s);
  void setToast() {}

public:
  CommandWidget(AppWindow *app);

public slots:
  virtual void onSearchChanged(const QString &) {}

signals:
  void replaceCommand(const Command *);

  // TODO: implement both
  // virtual void onActionSelected();
  // virtual void onActionActivated();
};
