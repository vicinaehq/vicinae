#include "ui/action-pannel/action-pannel-widget.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/action-pannel/static-action-pannel-list-view.hpp"
#include "ui/popover.hpp"
#include <qboxlayout.h>
#include <qlineedit.h>
#include <qstackedlayout.h>
#include <qstackedwidget.h>
#include <qwidget.h>

void ActionPannelWidget::closeEvent(QCloseEvent *event) {
  QWidget::closeEvent(event);
  emit closed();
}

void ActionPannelWidget::showEvent(QShowEvent *event) {
  QWidget::showEvent(event);
  emit opened();
}

void ActionPannelWidget::setSignalActions(const QList<AbstractAction *> &actions) {
  setActions({actions.begin(), actions.end()});
}

void ActionPannelWidget::setActions(const std::vector<ActionItem> &items) {
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
  setFixedHeight(window->height() * 0.45);
  auto x = parentGeo.width() - width() - 10;
  auto y = parentGeo.height() - height() - 50;
  QPoint global = window->mapToGlobal(QPoint(x, y));

  move(global);
  show();
}

bool ActionPannelWidget::eventFilter(QObject *obj, QEvent *event) {
  if (obj == _input && event->type() == QEvent::KeyPress) {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_Escape) {
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

bool ActionPannelWidget::findBoundAction(QKeyEvent *event) {
  qDebug() << "find bound action";
  if (_viewStack.empty()) return false;

  return false;
}

void ActionPannelWidget::connectView(ActionPannelView *view) {
  connect(view, &ActionPannelView::actionActivated, this, &ActionPannelWidget::actionExecuted);
}

void ActionPannelWidget::disconnectView(ActionPannelView *view) {
  disconnect(view, &ActionPannelView::actionActivated, this, &ActionPannelWidget::actionExecuted);
}

void ActionPannelWidget::pushView(ActionPannelView *view) {
  if (!_viewStack.empty()) { disconnectView(top()->view); }

  connectView(view);

  _viewLayout->addWidget(view);
  _viewStack.push_back({
      .text = _input->text(),
      .view = view,
  });
}

AbstractAction *ActionPannelWidget::primaryAction() const {
  if (_viewStack.empty()) return nullptr;

  auto actions = top()->view->actions();

  if (actions.empty()) return nullptr;

  return actions.at(0);
}

ActionPannelWidget::ActionPannelWidget(QWidget *parent)
    : Popover(parent), _input(new QLineEdit), _viewLayout(new QStackedWidget) {
  auto layout = new QVBoxLayout;

  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(_viewLayout);
  layout->addWidget(_input);
  _input->installEventFilter(this);

  setLayout(layout);
  connect(_input, &QLineEdit::textChanged, this, &ActionPannelWidget::textChanged);
}
