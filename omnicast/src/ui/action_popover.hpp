#pragma once
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "image-viewer.hpp"
#include "ui/keyboard.hpp"
#include "ui/virtual-list.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qhash.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qstack.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <qwidget.h>

class AppWindow;

class AbstractAction : public QObject {
  Q_OBJECT

public:
  QString title;
  ThemeIconModel icon;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  AbstractAction(const QString &title, const ThemeIconModel &icon = {}) : title(title), icon(icon) {}

  virtual void execute(AppWindow &app) = 0;

signals:
  void didExecute();
};

class ActionListItem : public AbstractVirtualListItem {
public:
  AbstractAction *action;

  QWidget *createItem() const override {
    auto iconLabel = new QLabel;
    auto label = new QLabel;
    auto layout = new QHBoxLayout;

    layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setText(action->title);

    layout->setSpacing(10);
    layout->addWidget(ImageViewer::createFromModel(action->icon, {20, 20}));
    layout->addWidget(label);
    layout->setContentsMargins(10, 0, 10, 0);

    auto widget = new QWidget;

    widget->setLayout(layout);

    return widget;
  }

  int height() const override { return 40; }

  QWidget *updateItem(QWidget *current) const override { return createItem(); }

public:
  ActionListItem(AbstractAction *action) : action(action) {}
};

class ActionListItemWidget : public QWidget {
  QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ActionListItemWidget(QWidget *image, const QString &name, const QString &category, const QString &kind,
                       QWidget *parent = nullptr)
      : QWidget(parent), icon(image), name(new QLabel), category(new QLabel), kind(new QLabel) {

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

enum ActionAfterActivateBehavior {
  ActionAfterActivateClose,
  ActionAfterActivateReset,
  ActionAfterActivateDoNothing,
};

struct ShownActionItem {
  QVariant data;
  ActionModel presentation;
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

  QList<ShownActionItem> shownActionItems;
  QList<std::shared_ptr<IAction>> _currentActions;
  QLineEdit *input;
  VirtualListWidget *list;
  VirtualListModel *listModel;
  QHash<QListWidgetItem *, AbstractAction *> signalItemMap;

  QStack<QList<ActionPannelItem>> menuStack;

  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void closeEvent(QCloseEvent *event) override {
    QWidget::closeEvent(event);
    emit closed();
  }

  void showEvent(QShowEvent *event) override {
    QWidget::showEvent(event);
    emit opened();
  }

private slots:
  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &item);

signals:
  void actionActivated(std::shared_ptr<IAction> action);
  void actionPressed(ActionModel model);
  void actionExecuted(AbstractAction *action);
  void closed() const;
  void opened() const;

public:
  QList<AbstractAction *> signalActions;

  bool findBoundAction(QKeyEvent *event) {
    for (auto action : signalActions) {
      if (!action->shortcut) continue;
      if (KeyboardShortcut(*action->shortcut) == event) {
        emit actionExecuted(action);
        return true;
      }
    }

    return false;
  }

  QList<ActionPannelItem> currentActions() const;

  void dispatchModel(const ActionPannelModel &model);
  void showActions();
  void toggleActions();
  void setActions(const QList<ActionPannelItem> &actions);

  void renderSignalItems(const QList<AbstractAction *> actions) {
    listModel->beginReset();

    for (const auto &item : actions) {
      QString str;

      if (item->shortcut) {
        QStringList lst;

        lst << item->shortcut->modifiers;
        lst << item->shortcut->key;
        str = lst.join(" + ");
      }

      listModel->addItem(std::make_shared<ActionListItem>(item));
    }

    listModel->endReset();
  }

  ActionPopover(QWidget *parent = 0);

public slots:
  void setSignalActions(const QList<AbstractAction *> &actions) { signalActions = actions; }
};
