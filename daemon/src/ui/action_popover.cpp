#include "action_popover.hpp"
#include "common.hpp"

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

  QPixmap *pix = 0;

  if (!pix || pix->isNull()) {
    pix = new QPixmap(
        QIcon::fromTheme("application-x-executable").pixmap(25, 25));
  }

  QHBoxLayout *layout = new QHBoxLayout(this);

  layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  layout->setSpacing(10);
  image->setPixmap(
      pix->scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  layout->addWidget(image);
  layout->addWidget(titleLabel, 1);
  // layout->addWidget(new KeyIndicator(action.bind));

  setLayout(layout);
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
      _list->setCurrentRow(i);
      reselected = true;
    }

    _list->item(i)->setHidden(!matches);
  }
}

void ActionPopover::itemActivated(QListWidgetItem *item) {
  if (!item)
    return;

  ActionItem *actionItem = (ActionItem *)_list->itemWidget(item);

  emit actionActivated(actionItem->action);
  hide();
}

ActionPopover::ActionPopover(QWidget *parent) : QWidget(parent) {
  setWindowFlags(Qt::Popup);

  auto layout = new QVBoxLayout(this);

  layout->setContentsMargins(0, 0, 0, 0);

  _list = new QListWidget();
  _input = new QLineEdit();
  _input->installEventFilter(this);

  connect(_input, &QLineEdit::textChanged, this, &ActionPopover::filterActions);

  layout->addWidget(_list);
  layout->addWidget(_input);

  setObjectName("action-popover");

  _list->setContentsMargins(0, 0, 0, 0);
  _list->setSpacing(0);
  _list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  _list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  _list->setSelectionMode(QAbstractItemView::SingleSelection);

  connect(_list, &QListWidget::itemActivated, this,
          &ActionPopover::itemActivated);

  _input->setPlaceholderText("Search actions");
  _input->setTextMargins(5, 5, 5, 5);

  setLayout(layout);
}

bool ActionPopover::eventFilter(QObject *obj, QEvent *event) {
  if (obj == _input && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
    QApplication::sendEvent(_list, keyEvent);

    return false;
  }

  return false;
}

void ActionPopover::setActions(const QList<std::shared_ptr<IAction>> &actions) {
  _currentActions = actions;
}

void ActionPopover::showActions() {
  _list->clear();

  for (const auto &action : _currentActions) {
    auto actionItem = new ActionItem(action);
    auto item = new QListWidgetItem(_list);

    item->setSizeHint(QSize(0, 45));
    _list->addItem(item);
    _list->setItemWidget(item, actionItem);
  }

  _input->setFocus();
  _input->clear();
  _list->setCurrentRow(0);

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
