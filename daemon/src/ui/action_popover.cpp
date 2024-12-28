#include "action_popover.hpp"
#include "command-object.hpp"
#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "extension.hpp"
#include "image-viewer.hpp"

#include <cctype>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qstyleoption.h>
#include <qwidget.h>

ActionItem::ActionItem(std::shared_ptr<IAction> action, QWidget *parent)
    : QWidget(parent), action(action) {
  setFixedHeight(45);
  setProperty("class", "action-item");
  image = new QLabel();
  titleLabel = new QLabel(action->name());
  titleLabel->setProperty("class", "action-name");
  resultType = new QLabel("Type");
  resultType->setProperty("class", "action-shortcut");

  QHBoxLayout *layout = new QHBoxLayout(this);

  layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  layout->setSpacing(10);
  image->setPixmap(action->icon().pixmap(25, 25));
  layout->addWidget(image);
  layout->addWidget(titleLabel, 1);
  // layout->addWidget(new KeyIndicator(action.bind));

  setLayout(layout);
}

void ActionPopover::toggleActions() {
  if (isVisible())
    hide();
  else
    showActions();
}

void ActionPopover::dispatchModel(const ActionPannelModel &model) {
  menuStack.clear();

  menuStack.push_back(model.children);
  renderItems(model.children);
}

QList<ActionPannelItem> ActionPopover::currentActions() const {
  if (menuStack.isEmpty())
    return {};

  return menuStack.top();
}

void ActionPopover::renderItems(const QList<ActionPannelItem> &items) {
  list->clear();
  itemMap.clear();

  for (const auto &item : items) {
    if (auto model = std::get_if<ActionModel>(&item)) {
      auto listItem = new QListWidgetItem;
      ImageLikeModel imageModel;

      if (!model->icon) {
        imageModel = ThemeIconModel{.iconName = "application-x-executable"};
      } else {
        imageModel = *model->icon;
      }

      QString str;

      if (model->shortcut) {
        QStringList lst;

        lst << model->shortcut->modifiers;
        lst << model->shortcut->key;
        str = lst.join(" + ");
      }

      auto widget = new ActionListItemWidget(
          ImageViewer::createFromModel(imageModel, {25, 25}), model->title, str,
          "");

      list->addItem(listItem);
      list->setItemWidget(listItem, widget);
      listItem->setSizeHint(widget->sizeHint());
      itemMap.insert(listItem, item);
    }
    if (auto model = std::get_if<ActionPannelSectionModel>(&item)) {
    }
    if (auto model = std::get_if<ActionPannelSubmenuModel>(&item)) {
      auto listItem = new QListWidgetItem;
      auto widget = new ActionListItemWidget(
          ImageViewer::createFromModel(*model->icon, {25, 25}), model->title,
          "", "");

      list->addItem(listItem);
      list->setItemWidget(listItem, widget);
      listItem->setSizeHint(widget->sizeHint());
      itemMap.insert(listItem, item);
    }
  }

  for (int i = 0; i != list->count(); ++i) {
    auto item = list->item(i);

    if (!item->flags().testFlag(Qt::ItemIsSelectable))
      continue;

    list->setCurrentItem(item);
    break;
  }
}

void ActionPopover::paintEvent(QPaintEvent *event) {
  QStyleOption o;
  QPainter p(this);

  o.initFrom(this);
  style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void ActionPopover::filterActions(const QString &text) {
  if (menuStack.isEmpty())
    return;

  QList<ActionPannelItem> items;

  for (const auto &item : menuStack.top()) {
    if (auto model = std::get_if<ActionModel>(&item);
        model->title.contains(text, Qt::CaseInsensitive)) {
      items.push_back(item);
    }
  }

  renderItems(items);
}

void ActionPopover::itemActivated(QListWidgetItem *item) {
  if (!item || !itemMap.contains(item))
    return;

  auto action = itemMap.value(item);

  if (auto model = std::get_if<ActionModel>(&action)) {
    qDebug() << "selected action with title" << model->title;
    emit actionPressed(*model);
  }

  hide();
}

ActionPopover::ActionPopover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::Popup);

  auto layout = new QVBoxLayout(this);

  layout->setContentsMargins(0, 0, 0, 0);

  list = new QListWidget();
  input = new QLineEdit();
  input->installEventFilter(this);

  connect(input, &QLineEdit::textChanged, this, &ActionPopover::filterActions);

  layout->addWidget(list);
  layout->addWidget(input);

  setObjectName("action-popover");

  list->setContentsMargins(0, 0, 0, 0);
  list->setSpacing(0);
  list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  list->setSelectionMode(QAbstractItemView::SingleSelection);
  list->setFocusPolicy(Qt::NoFocus);

  connect(list, &QListWidget::itemActivated, this,
          &ActionPopover::itemActivated);

  input->setPlaceholderText("Search actions");
  input->setTextMargins(5, 5, 5, 5);

  setLayout(layout);
}

bool ActionPopover::eventFilter(QObject *obj, QEvent *event) {
  if (obj == input && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    QApplication::sendEvent(list, keyEvent);

    return false;
  }

  return false;
}

void ActionPopover::setActions(const QList<ActionPannelItem> &actions) {
  menuStack.clear();
  menuStack.push(actions);
}

void ActionPopover::selectPrimary() {
  for (const auto &item : menuStack.top()) {
    if (auto model = std::get_if<ActionModel>(&item)) {
      emit actionPressed(*model);
      return;
    }
    if (auto model = std::get_if<ActionPannelSubmenuModel>(&item)) {
    }
    if (auto model = std::get_if<ActionPannelSectionModel>(&item)) {
      if (model->actions.isEmpty()) {
        auto &action = model->actions.at(0);

        emit actionPressed(action);
        return;
      }
    }
  }
}

void ActionPopover::showActions() {
  input->setFocus();
  input->clear();

  auto window = QApplication::activeWindow();

  if (!window) {
    qDebug() << "showActions: no active window, won't show popover";
    return;
  }

  qDebug() << "creating list with " << window->width() << "x"
           << window->height();

  adjustSize();

  auto parentGeo = window->geometry();

  qDebug() << "width=" << parentGeo.width() << " height=" << parentGeo.height();
  qDebug() << "width2=" << width() << " height2=" << height();
  qDebug() << "width2=" << geometry().width()
           << " height2=" << geometry().height();

  setMinimumWidth(window->width() * 0.45);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = window->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}
