#pragma once
#include "ui/action-pannel/action-pannel-view.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/popover.hpp"
#include <qlineedit.h>
#include <qstackedlayout.h>
#include <qtmetamacros.h>
#include <QStackedWidget>

struct ActionPannelViewSnapshot {
  QString text;
  ActionPannelView *view;
};

class ActionPannelWidget : public Popover {
public:
  using ViewStack = std::vector<ActionPannelViewSnapshot>;

private:
  Q_OBJECT

  QLineEdit *_input;
  QStackedWidget *_viewLayout;
  ViewStack _viewStack;

  bool eventFilter(QObject *obj, QEvent *event) override;
  void closeEvent(QCloseEvent *event) override;
  void showEvent(QShowEvent *event) override;

  const ActionPannelViewSnapshot *top() const;

public:
  AbstractAction *findBoundAction(QKeyEvent *event);

  void showActions();
  void toggleActions();

  void textChanged(const QString &text) const;
  void clear();
  void popCurrentView();
  void connectView(ActionPannelView *view);
  void disconnectView(ActionPannelView *view);
  void pushView(ActionPannelView *view);
  void popToRoot();

  ViewStack takeViewStack();
  void restoreViewStack(const ViewStack &stack);

  void setSignalActions(const QList<AbstractAction *> &actions);
  void setActions(std::vector<ActionItem> items);
  AbstractAction *primaryAction() const;

  ActionPannelWidget(QWidget *parent = 0);

signals:
  void actionExecuted(AbstractAction *action);
  void closed() const;
  void opened() const;
};
