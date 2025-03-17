#include "action_popover.hpp"
#include "common.hpp"
#include "extend/action-model.hpp"
#include "theme.hpp"
#include "ui/omni-list.hpp"

#include <QPainterPath>
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

  backgroundColor.setAlphaF(0.98);

  painter.fillPath(path, backgroundColor);

  // Draw the border
  QPen pen(theme.colors.border, 1); // Border with a thickness of 2
  painter.setPen(pen);
  painter.drawPath(path);
}

void ActionPopover::itemActivated(const OmniList::AbstractVirtualItem &item) {
  auto actionItem = static_cast<const ActionListItem &>(item);

  emit actionExecuted(actionItem.action);
  close();
}

ActionPopover::ActionPopover(QWidget *parent) : QWidget(parent), list(new OmniList) {
  setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
  setAttribute(Qt::WA_TranslucentBackground);

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

  connect(list, &OmniList::itemActivated, this, &ActionPopover::itemActivated);

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

  renderSignalItems(signalActions);
  adjustSize();

  auto parentGeo = window->geometry();

  setMinimumWidth(window->width() * 0.5);
  setFixedHeight(window->height() * 0.45);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = window->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}
