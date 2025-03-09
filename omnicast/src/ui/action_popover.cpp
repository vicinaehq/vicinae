#include "action_popover.hpp"
#include "common.hpp"
#include "extend/action-model.hpp"
#include "theme.hpp"
#include "ui/virtual-list.hpp"

#include <QPainterPath>
#include <cctype>
#include <memory>
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

void ActionPopover::toggleActions() {
  if (isVisible())
    close();
  else
    showActions();
}

void ActionPopover::dispatchModel(const ActionPannelModel &model) {
  menuStack.clear();

  menuStack.push_back(model.children);
  // renderItems(model.children);
}

QList<ActionPannelItem> ActionPopover::currentActions() const {
  if (menuStack.isEmpty()) return {};

  return menuStack.top();
}

void ActionPopover::paintEvent(QPaintEvent *event) {
  int borderRadius = 10;
  auto &theme = ThemeService::instance().theme();

  QPainter painter(this);

  painter.setRenderHint(QPainter::Antialiasing, true);

  QPainterPath path;
  path.addRoundedRect(rect(), borderRadius, borderRadius);

  painter.setClipPath(path);

  QColor backgroundColor(theme.colors.mainBackground);

  painter.fillPath(path, backgroundColor);

  // Draw the border
  QPen pen(theme.colors.border, 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);
}

void ActionPopover::itemActivated(const std::shared_ptr<AbstractVirtualListItem> &item) {
  auto actionItem = std::static_pointer_cast<ActionListItem>(item);

  emit actionExecuted(actionItem->action);
  close();
}

ActionPopover::ActionPopover(QWidget *parent)
    : QWidget(parent), list(new VirtualListWidget), listModel(new VirtualListModel) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);

  list->setModel(listModel);

  auto layout = new QVBoxLayout(this);

  layout->setContentsMargins(0, 0, 0, 0);

  input = new QLineEdit();
  input->installEventFilter(this);

  // connect(input, &QLineEdit::textChanged, this, &ActionPopover::filterActions);

  auto listContainer = new QWidget;
  auto listLayout = new QVBoxLayout;

  listLayout->setContentsMargins(0, 0, 0, 0);
  listLayout->addWidget(list);
  listContainer->setLayout(listLayout);

  layout->addWidget(listContainer);

  input->setContentsMargins(15, 15, 15, 15);

  layout->setSpacing(0);

  layout->addWidget(listContainer);
  layout->addWidget(new HDivider);
  layout->addWidget(input);

  connect(list, &VirtualListWidget::itemActivated, this, &ActionPopover::itemActivated);

  input->setPlaceholderText("Search actions");

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

void ActionPopover::showActions() {
  input->setFocus();
  input->clear();

  auto window = QApplication::activeWindow();

  if (!window) {
    qDebug() << "showActions: no active window, won't show popover";
    return;
  }

  qDebug() << "creating list with " << window->width() << "x" << window->height();

  renderSignalItems(signalActions);
  adjustSize();

  auto parentGeo = window->geometry();

  qDebug() << "width=" << parentGeo.width() << " height=" << parentGeo.height();
  qDebug() << "width2=" << width() << " height2=" << height();
  qDebug() << "width2=" << geometry().width() << " height2=" << geometry().height();

  setMinimumWidth(window->width() * 0.45);
  setFixedHeight(200);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = window->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}
