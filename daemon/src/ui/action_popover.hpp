#pragma once
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "image-viewer.hpp"
#include "ui/keyboard.hpp"
#include "ui/status_bar.hpp"
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
public:
  QString title;
  ThemeIconModel icon;
  std::optional<KeyboardShortcutModel> shortcut;

  void setShortcut(const KeyboardShortcutModel &shortcut) {
    this->shortcut = shortcut;
  }

  AbstractAction(const QString &title, const ThemeIconModel &icon = {})
      : title(title), icon(icon) {}

  virtual void execute(AppWindow &app) = 0;
};

struct ActionData {
  QString title;
  ThemeIconModel icon;
  std::function<void(void)> execute;
  std::optional<KeyboardShortcutModel> shortcut;
};

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

class NewActionPannelModel : public QObject {
  Q_OBJECT
  QList<QVariant> items;

public:
  void setItems(const QList<QVariant> &items) {
    this->items = items;
    emit itemsChanged(items);
  }

  const QVariant &row(int index) { return this->items.at(index); }

signals:
  void itemsChanged(const QList<QVariant> &items);
};

template <typename T>
class VariantActionPannelModel : public NewActionPannelModel {
public:
  void setItems(const QList<T> &items) {
    QList<QVariant> variants;

    for (const auto &item : items) {
      variants.push_back(QVariant::fromValue(item));
    }

    NewActionPannelModel::setItems(variants);
  }
};

class AbstractActionItemDelegate {
public:
  virtual ActionModel present(const QVariant &data) = 0;
  virtual KeyboardShortcutModel primaryActionShortcut() {
    return KeyboardShortcutModel{
        .key = "return",
    };
  }
  virtual KeyboardShortcutModel secondaryActionShortcut() {
    return KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}};
  }
  virtual void activate(const QVariant &data) {};
};

template <class T>
class TypedActionDelegate : public AbstractActionItemDelegate {
  ActionModel present(const QVariant &data) override {
    return presentVariant(data.value<T>());
  }

  void activate(const QVariant &data) override {
    return activateVariant(data.value<T>());
  }

public:
  TypedActionDelegate() {}

  virtual ActionModel presentVariant(const T &action) {};
  virtual void activateVariant(const T &action) {};
};

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

  NewActionPannelModel *m_model;
  AbstractActionItemDelegate *m_delegate;

  QList<ShownActionItem> shownActionItems;
  QList<std::shared_ptr<IAction>> _currentActions;
  QLineEdit *input;
  QListWidget *list;
  QList<ActionData> actionData;
  QHash<QListWidgetItem *, ActionData> itemMap;
  QHash<QListWidgetItem *, AbstractAction *> signalItemMap;

  QStack<QList<ActionPannelItem>> menuStack;

  QList<AbstractAction *> signalActions;

  void paintEvent(QPaintEvent *event) override;
  bool eventFilter(QObject *obj, QEvent *event) override;

  void renderItems(const QList<ActionData> &items);

private slots:
  void filterActions(const QString &text);
  void itemActivated(QListWidgetItem *item);

  void modelItemsChanged(const QList<QVariant> &items) {
    QList<ActionPannelItem> finalItems;
    size_t index = 0;

    shownActionItems.clear();

    for (const auto &item : items) {
      auto presentation = m_delegate->present(item);

      switch (finalItems.size()) {
      case 0:
        presentation.shortcut = m_delegate->primaryActionShortcut();
        break;
      case 1:
        presentation.shortcut = m_delegate->secondaryActionShortcut();
        break;
      }

      shownActionItems.push_back({.data = item, .presentation = presentation});
      finalItems.push_back(presentation);
    }
  }

signals:
  void actionActivated(std::shared_ptr<IAction> action);
  void actionPressed(ActionModel model);
  void actionExecuted(AbstractAction *action);

public:
  bool submitKeypress(QKeyEvent *event) {
    for (const auto &action : actionData) {
      if (!action.shortcut)
        continue;
      if (KeyboardShortcut(*action.shortcut) == event) {
        action.execute();
        return true;
      }
    }

    return false;
  }

  AbstractActionItemDelegate *delegate() const { return m_delegate; }

  NewActionPannelModel *model() const { return m_model; }

  void setDelegate(AbstractActionItemDelegate *delegate) {
    this->m_delegate = delegate;
  }

  void setModel(NewActionPannelModel *model) {
    this->m_model = model;
    connect(this->m_model, &NewActionPannelModel::itemsChanged, this,
            &ActionPopover::modelItemsChanged);
  }

  QList<ActionPannelItem> currentActions() const;
  QList<ActionData> actions() const { return actionData; }

  void selectPrimary();
  void dispatchModel(const ActionPannelModel &model);
  void showActions();
  void toggleActions();
  void setActions(const QList<ActionPannelItem> &actions);

  void renderSignalItems(const QList<AbstractAction *> actions) {
    list->clear();
    itemMap.clear();

    for (const auto &item : actions) {
      auto listItem = new QListWidgetItem;
      ImageLikeModel imageModel;

      imageModel = item->icon;

      QString str;

      if (item->shortcut) {
        QStringList lst;

        lst << item->shortcut->modifiers;
        lst << item->shortcut->key;
        str = lst.join(" + ");
      }

      auto widget = new ActionListItemWidget(
          ImageViewer::createFromModel(imageModel, {25, 25}), item->title, "",
          str);

      list->addItem(listItem);
      list->setItemWidget(listItem, widget);
      listItem->setSizeHint(widget->sizeHint());
      signalItemMap.insert(listItem, item);
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

  ActionPopover(QWidget *parent = 0);

public slots:
  void setActionData(const QList<ActionData> &actions) { actionData = actions; }
  void setSignalActions(const QList<AbstractAction *> &actions) {
    signalActions = actions;
  }
};
