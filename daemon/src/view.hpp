#pragma once
#include "app.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class View : public QObject {
  Q_OBJECT
  AppWindow &app;
  QList<QWidget *> inputFwdTo;

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (app.topBar->input && event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      auto key = keyEvent->key();

      if (key == Qt::Key_Return && app.topBar->quickInput) {
        if (app.topBar->quickInput->focusFirstEmpty())
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

protected:
public:
  QWidget *widget;
  View(AppWindow &app) : app(app) {}

  void forwardInputEvents(QWidget *widget) {
    for (const auto &w : inputFwdTo) {
      if (widget == w)
        return;
    }

    inputFwdTo.push_back(widget);
  }

  template <typename T> Service<T> service() { return app.service<T>(); }
  void setSearchPlaceholderText(const QString &s) {
    app.topBar->input->setPlaceholderText(s);
  }
  void setActions(const ActionPannelModel &model) {
    app.actionPopover->dispatchModel(model);

    if (!model.children.isEmpty()) {
      auto &first = model.children.at(0);
      app.statusBar->setCurrentAction(first);
    }
  }

  void hideInput() {
    app.topBar->input->hide();
    app.topBar->input->setReadOnly(true);
    app.topBar->input->setText("");
  }

public slots:
  virtual void onSearchChanged(const QString &s) {}
  virtual void onActionActivated(ActionModel model) {}

signals:
  void launchCommand(ViewCommand *command);
  void pushView(View *view);
  void pop();
  void popToRoot();
};
