#pragma once
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "image-viewer.hpp"
#include "omni-icon.hpp"
#include "theme.hpp"
#include "ui/keyboard-shortcut-indicator.hpp"
#include "ui/keyboard.hpp"
#include "ui/omni-list-item-widget.hpp"
#include "ui/omni-list.hpp"
#include "ui/selectable-omni-list-widget.hpp"
#include "ui/virtual-list.hpp"
#include <qboxlayout.h>
#include <qevent.h>
#include <qhash.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlistwidget.h>
#include <qnamespace.h>
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
  OmniIconUrl iconUrl;
  std::optional<KeyboardShortcutModel> shortcut;
  std::function<void(void)> _execCallback;

  void setShortcut(const KeyboardShortcutModel &shortcut) { this->shortcut = shortcut; }
  void setExecutionCallback(const std::function<void(void)> &cb) { _execCallback = cb; }

  std::function<void(void)> executionCallback() const { return _execCallback; }

  AbstractAction(const QString &title, const OmniIconUrl &icon) : title(title), iconUrl(icon) {}

  virtual void execute(AppWindow &app) = 0;

signals:
  void didExecute();
};

class ActionListWidget : public SelectableOmniListWidget {
  OmniIcon *_icon;
  QLabel *_label;
  KeyboardShortcutIndicatorWidget *_shortcut;

public:
  ActionListWidget &setIconUrl(const OmniIconUrl &url) {
    _icon->setUrl(url);
    return *this;
  }

  ActionListWidget &setShortcut(const KeyboardShortcutModel &shortcut) {
    _shortcut->setShortcut(shortcut);
    _shortcut->show();
    return *this;
  }

  ActionListWidget &clearShortcut() {
    _shortcut->hide();
    return *this;
  }

  ActionListWidget &setTitle(const QString &title) {
    _label->setText(title);
    return *this;
  }

  void selectionChanged(bool selected) override {
    SelectableOmniListWidget::selectionChanged(selected);
    auto &theme = ThemeService::instance().theme();

    if (selected) {
      _shortcut->setBackgroundColor(theme.colors.statusBackground);
    } else {
      _shortcut->setBackgroundColor(theme.colors.statusBackground);
    }
  }

  ActionListWidget()
      : _icon(new OmniIcon), _label(new QLabel), _shortcut(new KeyboardShortcutIndicatorWidget) {
    auto &theme = ThemeService::instance().theme();
    auto layout = new QHBoxLayout;

    _shortcut->hide();
    _shortcut->setBackgroundColor(theme.colors.statusBackground);

    _icon->setFixedSize(22, 22);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(10);
    layout->addWidget(_icon);
    layout->addWidget(_label);
    layout->addWidget(_shortcut, 0, Qt::AlignRight);
    layout->setContentsMargins(8, 8, 8, 8);

    setLayout(layout);
  }
};

class ActionListItem : public OmniList::AbstractVirtualItem {
public:
  AbstractAction *action;

  QString id() const override { return action->title + action->iconUrl.toString(); }

  OmniListItemWidget *createWidget() const override {
    auto widget = new ActionListWidget;

    setup(widget);

    return widget;
  }

  int calculateHeight(int width) const override {
    static ActionListWidget ruler;

    return ruler.sizeHint().height();
  }

  bool recyclable() const override { return true; }

  void setup(ActionListWidget *widget) const {
    widget->setTitle(action->title).setIconUrl(action->iconUrl);

    if (auto shortcut = action->shortcut) {
      widget->setShortcut(*shortcut);
    } else {
      widget->clearShortcut();
    }
  }

  void recycle(QWidget *base) const override { setup(static_cast<ActionListWidget *>(base)); }

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
  OmniList *list;
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
  void itemActivated(const OmniList::AbstractVirtualItem &item);

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
    std::vector<std::unique_ptr<OmniList::AbstractVirtualItem>> items;

    for (const auto &item : actions) {
      QString str;

      if (item->shortcut) {
        QStringList lst;

        lst << item->shortcut->modifiers;
        lst << item->shortcut->key;
        str = lst.join(" + ");
      }

      items.push_back(std::make_unique<ActionListItem>(item));
    }

    list->updateFromList(items, OmniList::SelectionPolicy::SelectFirst);
  }

  ActionPopover(QWidget *parent = 0);

public slots:
  void setSignalActions(const QList<AbstractAction *> &actions) { signalActions = actions; }
};
