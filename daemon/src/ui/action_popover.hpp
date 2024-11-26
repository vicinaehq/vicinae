#pragma once
#include "common.hpp"
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpixmap.h>
#include <qwidget.h>

class SearchResult;

typedef std::function<void(void)> ActionHandler;

enum ActionAfterActivateBehavior {
  ActionAfterActivateClose,
  ActionAfterActivateReset,
  ActionAfterActivateDoNothing,
};

class ActionItem : public QWidget {
  Q_OBJECT
  QLabel *titleLabel;
  QLabel *image;
  QLabel *resultType;

public:
  std::shared_ptr<IAction> action;

  ActionItem(std::shared_ptr<IAction> action, QWidget *parent = 0);
};

class ActionPopover : public QWidget {
  Q_OBJECT

  QList<std::shared_ptr<IAction>> _currentActions;
  QLineEdit *_input;
  QListWidget *_list;

  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
  void filterActions(const QString &text);
  void itemActivated(QListWidgetItem *item);

signals:
  void actionActivated(std::shared_ptr<IAction> action);

public:
  void showActions();
  void toggleActions();
  void setActions(const QList<std::shared_ptr<IAction>> &actions);

  ActionPopover(QWidget *parent = 0);
};
