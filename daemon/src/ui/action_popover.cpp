#include "action_popover.hpp"
#include "command-object.hpp"
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
  list->clear();

  if (!model.title.isEmpty()) {
    list->addItem(model.title);
  }

  for (const auto &child : model.children) {
    if (auto model = std::get_if<ActionModel>(&child)) {
      auto listItem = new QListWidgetItem;
      auto widget = new ActionListItemWidget(
          ImageViewer::createFromModel(*model->icon, {25, 25}), model->title,
          "", "");

      list->addItem(listItem);
      list->setItemWidget(listItem, widget);
      listItem->setSizeHint(widget->sizeHint());
    }
    if (auto model = std::get_if<ActionPannelSectionModel>(&child)) {
    }
    if (auto model = std::get_if<ActionPannelSubmenuModel>(&child)) {
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
  bool reselected = false;

  for (size_t i = 0; i != _currentActions.size(); ++i) {
    auto matches =
        _currentActions[i]->name().toLower().contains(text.toLower());

    if (!reselected && matches) {
      list->setCurrentRow(i);
      reselected = true;
    }

    list->item(i)->setHidden(!matches);
  }
}

void ActionPopover::itemActivated(QListWidgetItem *item) {
  if (!item)
    return;

  ActionItem *actionItem = (ActionItem *)list->itemWidget(item);

  emit actionActivated(actionItem->action);
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

void ActionPopover::setActions(const QList<std::shared_ptr<IAction>> &actions) {
  _currentActions = actions;
}

void ActionPopover::showActions() {
  input->setFocus();
  input->clear();

  adjustSize();

  auto parentGeo = parentWidget()->geometry();

  qDebug() << "width=" << parentGeo.width() << " height=" << parentGeo.height();
  qDebug() << "width2=" << width() << " height2=" << height();
  qDebug() << "width2=" << geometry().width()
           << " height2=" << geometry().height();

  setMinimumWidth(parentWidget()->width() * 0.45);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = parentWidget()->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}
