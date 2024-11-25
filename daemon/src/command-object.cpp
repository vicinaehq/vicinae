#include "command-object.hpp"
#include "omnicast.hpp"
#include <qwidget.h>

CommandObject::CommandObject(AppWindow *app) : app(app), widget(new QWidget()) {
  setObjectName("CommandObject");
  connect(app->topBar->input, &QLineEdit::textChanged, this,
          &CommandObject::onSearchChanged);
  connect(app->actionPopover, &ActionPopover::actionActivated, this,
          &CommandObject::onActionActivated);

  app->topBar->input->installEventFilter(this);
}

CommandObject::~CommandObject() { widget->deleteLater(); }

bool CommandObject::eventFilter(QObject *obj, QEvent *event) {
  if (obj == searchbar() && event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);
    auto key = keyEvent->key();

    if (key == Qt::Key_Return && app->topBar->quickInput) {
      if (app->topBar->quickInput->focusFirstEmpty())
        return true;
    }

    switch (key) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
    case Qt::Key_Enter:
      for (const auto &widget : inputFwdTo) {
        QApplication::sendEvent(widget, event);
      }
      break;
    default:
      break;
    }
  }

  return false;
}

void CommandObject::setActions(const QList<std::shared_ptr<IAction>> &actions) {
  app->actionPopover->setActions(actions);

  if (!actions.isEmpty())
    app->statusBar->setSelectedAction(actions.at(0));
}

void CommandObject::createCompletion(const QList<QString> &inputs,
                                     const QString &icon) {
  app->topBar->activateQuicklinkCompleter(inputs);
  app->topBar->quickInput->setIcon(icon);
}

void CommandObject::destroyCompletion() {
  if (app->topBar->quickInput) {
    app->topBar->destroyQuicklinkCompleter();
  }
}

QLineEdit *CommandObject::searchbar() { return app->topBar->input; }

void CommandObject::setSearchPlaceholder(const QString &s) {
  searchbar()->setPlaceholderText(s);
}

void CommandObject::forwardInputEvents(QWidget *widget) {
  for (const auto &w : inputFwdTo) {
    if (widget == w)
      return;
  }

  inputFwdTo.push_back(widget);
}

void CommandObject::unforwardInputEvents(QWidget *widget) {
  for (const auto &w : inputFwdTo) {
    if (widget == w) {
      inputFwdTo.removeOne(w);
    }
  }
}

void CommandObject::clearSearch() { app->topBar->input->clear(); }

void CommandObject::setSearch(const QString &s) { app->topBar->input->clear(); }

void CommandObject::setToast(const QString &message, ToastPriority priority) {
  app->statusBar->setToast(message, priority);
}

QString CommandObject::query() const { return app->topBar->input->text(); }

QList<QString> CommandObject::completions() const {
  QList<QString> completions;

  if (auto completer = app->topBar->quickInput) {
    for (const auto &input : completer->inputs) {
      completions.push_back(input->text());
    }
  }

  return completions;
}
