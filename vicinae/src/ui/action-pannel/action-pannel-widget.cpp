#include "ui/action-pannel/action-pannel-widget.hpp"
#include "common.hpp"
#include "ui/action-pannel/action-item.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/static-action-pannel-list-view.hpp"
#include "ui/keyboard.hpp"
#include "ui/popover/popover.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qstackedlayout.h>
#include <qstackedwidget.h>
#include <qwidget.h>

void ActionPannelWidget::closeEvent(QCloseEvent *event) {
  QWidget::closeEvent(event);
  popToRoot();
  emit closed();
}

void ActionPannelWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  emit opened();
}

void ActionPannelWidget::setSignalActions(const QList<AbstractAction *> &actions) {
  std::vector<ActionItem> items;

  items.reserve(actions.size());

  for (const auto &action : actions) {
    items.push_back(std::shared_ptr<AbstractAction>(action));
  }

  setActions(items);
}

void ActionPannelWidget::setActions(std::vector<ActionItem> items) {
  clear();
  pushView(new StaticActionPannelListView(items));
}

void ActionPannelWidget::textChanged(const QString &text) const {
  if (_viewStack.empty()) return;

  top()->view->onSearchChanged(text);
}

void ActionPannelWidget::clear() {
  while (!_viewStack.empty()) {
    popCurrentView();
  }
}

ActionPannelWidget::ViewStack ActionPannelWidget::takeViewStack() {
  ViewStack stack = _viewStack;

  for (auto it = _viewStack.rbegin(); it != _viewStack.rend(); ++it) {
    _viewLayout->removeWidget(it->view);
  }
  _viewStack.clear();

  return stack;
}

void ActionPannelWidget::restoreViewStack(const ActionPannelWidget::ViewStack &stack) {
  takeViewStack();

  for (auto it = stack.begin(); it != stack.end(); ++it) {
    _viewLayout->addWidget(it->view);
  }

  _viewStack = stack;
}

const ActionPannelViewSnapshot *ActionPannelWidget::top() const {
  if (_viewStack.empty()) return nullptr;

  return &_viewStack.at(_viewStack.size() - 1);
}

void ActionPannelWidget::popCurrentView() {
  if (_viewStack.empty()) return;

  auto snap = top();

  _input->setText(snap->text);
  _viewLayout->removeWidget(snap->view);
  snap->view->deleteLater();
  _viewStack.pop_back();

  if (!_viewStack.empty()) { connectView(top()->view); }
}

void ActionPannelWidget::showActions() {
  _input->setFocus();
  _input->clear();

  auto window = QApplication::activeWindow();

  if (!window) {
    qDebug() << "showActions: no active window, won't show popover";
    return;
  }

  adjustSize();

  auto parentGeo = window->geometry();

  setMinimumWidth(window->width() * 0.5);
  // setFixedHeight(window->height() * 0.45);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = window->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}

bool ActionPannelWidget::eventFilter(QObject *obj, QEvent *event) {
  if (obj == _input && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_Escape ||
        (keyEvent->key() == Qt::Key_Backspace && _input->text().isEmpty())) {
      if (_viewStack.size() > 1) {
        popCurrentView();
      } else {
        close();
      }

      return true;
    }

    if (!_viewStack.empty()) {
      qDebug() << "pass to topmost view";
      switch (keyEvent->key()) {
      case Qt::Key_Up:
      case Qt::Key_Down:
      case Qt::Key_Left:
      case Qt::Key_Right:
      case Qt::Key_Return:
        QApplication::sendEvent(top()->view, event);
        return true;
      }
    }

    return false;
  }

  return false;
}

AbstractAction *ActionPannelWidget::findBoundAction(QKeyEvent *event) {
  if (_viewStack.empty()) return nullptr;

  for (auto it = _viewStack.rbegin(); it != _viewStack.rend(); ++it) {
    for (auto action : it->view->actions()) {
      if (action->shortcut && KeyboardShortcut(*action->shortcut) == event) { return action.get(); }
    }
  }

  return nullptr;
}

void ActionPannelWidget::popToRoot() {
  while (_viewStack.size() > 1) {
    popCurrentView();
  }

  _input->clear();
  emit _input->textEdited("");
}

void ActionPannelWidget::connectView(ActionPanelView *view) {
  connect(view, &ActionPanelView::actionActivated, this, &ActionPannelWidget::actionExecuted);
}

void ActionPannelWidget::disconnectView(ActionPanelView *view) {
  disconnect(view, &ActionPanelView::actionActivated, this, &ActionPannelWidget::actionExecuted);
}

void ActionPannelWidget::pushView(ActionPanelView *view) {
  if (!_viewStack.empty()) { disconnectView(top()->view); }

  connectView(view);

  _viewLayout->addWidget(view);
  _viewLayout->setCurrentWidget(view);
  _viewStack.push_back({
      .text = _input->text(),
      .view = view,
  });
  _input->clear();
  emit _input->textEdited("");
}

AbstractAction *ActionPannelWidget::primaryAction() const {
  if (_viewStack.empty()) return nullptr;

  auto actions = top()->view->actions();

  for (const auto &action : actions) {
    if (action->isPrimary()) return action.get();
  }

  return nullptr;
}

ActionPannelWidget::ActionPannelWidget(QWidget *parent)
    : Popover(parent), _input(new QLineEdit), _viewLayout(new QStackedWidget) {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(_viewLayout);
  layout->addWidget(new HDivider);
  layout->addWidget(_input);
  layout->setSpacing(0);
  _input->installEventFilter(this);
  _viewLayout->setContentsMargins(0, 0, 0, 0);

  _input->setContentsMargins(10, 10, 10, 10);
  _input->setPlaceholderText("Filter actions");

  setLayout(layout);
  connect(_input, &QLineEdit::textEdited, this, &ActionPannelWidget::textChanged);
}
