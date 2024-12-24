#pragma once
#include "extend/action-model.hpp"
#include <qboxlayout.h>
#include <qhash.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qpixmap.h>
#include <qstack.h>
#include <qwidget.h>

class ActionListItemWidget : public QWidget {
  QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ActionListItemWidget(QWidget *image, const QString &name,
                       const QString &category, const QString &kind,
                       QWidget *parent = nullptr)
      : QWidget(parent), icon(image), name(new QLabel), category(new QLabel),
        kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);
  }
};

class SearchResult;
class IAction;

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
  QLineEdit *input;
  QListWidget *list;
  QHash<QListWidgetItem *, ActionPannelItem> itemMap;

  QStack<QList<ActionPannelItem>> menuStack;

  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void renderItems(const QList<ActionPannelItem> &items);

private slots:
  void filterActions(const QString &text);
  void itemActivated(QListWidgetItem *item);

signals:
  void actionActivated(std::shared_ptr<IAction> action);
  void actionPressed(ActionModel model);

public:
  void dispatchModel(const ActionPannelModel &model);
  void showActions();
  void toggleActions();
  void setActions(const QList<std::shared_ptr<IAction>> &actions);

  ActionPopover(QWidget *parent = 0);
};
