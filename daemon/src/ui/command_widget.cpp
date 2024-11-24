#include "ui/command_widget.hpp"
#include "omnicast.hpp"

CommandWidget::CommandWidget(AppWindow *app) : app(app) {
  setObjectName("CommandWidget");
  connect(app->topBar->input, &QLineEdit::textChanged, this,
          &CommandWidget::onSearchChanged);

  app->topBar->input->installEventFilter(this);
}

bool CommandWidget::eventFilter(QObject *obj, QEvent *event) {
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

void CommandWidget::createCompletion(const QList<QString> &inputs) {
  app->topBar->activateQuicklinkCompleter(inputs);
}

void CommandWidget::destroyCompletion() {
  if (app->topBar->quickInput) {
    app->topBar->destroyQuicklinkCompleter();
  }
}

QLineEdit *CommandWidget::searchbar() { return app->topBar->input; }

void CommandWidget::setSearchPlaceholder(const QString &s) {
  searchbar()->setPlaceholderText(s);
}

void CommandWidget::forwardInputEvents(QWidget *widget) {
  for (const auto &w : inputFwdTo) {
    if (widget == w)
      return;
  }

  inputFwdTo.push_back(widget);
}

void CommandWidget::unforwardInputEvents(QWidget *widget) {
  for (const auto &w : inputFwdTo) {
    if (widget == w) {
      inputFwdTo.removeOne(w);
    }
  }
}

void CommandWidget::clearSearch() { app->topBar->input->clear(); }

void CommandWidget::setSearch(const QString &s) { app->topBar->input->clear(); }
