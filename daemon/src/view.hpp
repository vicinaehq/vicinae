#pragma once
#include "app.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qcompare.h>
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
  QList<IInputHandler *> inputHandlers;

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (obj == app.topBar->input && event->type() == QEvent::KeyPress) {
      auto keyEvent = static_cast<QKeyEvent *>(event);
      auto key = keyEvent->key();

      // qDebug() << "key event from view filter";

      if (key == Qt::Key_Return && app.topBar->quickInput) {
        if (app.topBar->quickInput->focusFirstEmpty())
          return true;
      }

      switch (key) {
      case Qt::Key_Up:
      case Qt::Key_Down:
        for (const auto &widget : inputFwdTo) {
          QApplication::sendEvent(widget, event);
        }
        for (auto handler : inputHandlers) {
          handler->handleInput(keyEvent);
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

  void addInputHandler(IInputHandler *widget) {
    inputHandlers.push_back(widget);
  }

  template <typename T> Service<T> service() { return app.service<T>(); }
  void setSearchPlaceholderText(const QString &s) {
    app.topBar->input->setPlaceholderText(s);
  }
  void setActions(const QList<ActionData> &actions) {
    QList<ActionData> newActions = actions;

    if (newActions.size() > 0) {
      newActions[0].shortcut =
          KeyboardShortcutModel{.key = "return", .modifiers = {}};
    }

    if (newActions.size() > 1) {
      newActions[1].shortcut =
          KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}};
    }

    app.actionPopover->setActionData(newActions);

    if (!actions.isEmpty()) {
      // app.statusBar->setCurrentAction(actions.at(0).title);
    }
  }

  void setSignalActions(const QList<AbstractAction *> &actions) {
    app.actionPopover->setSignalActions(actions);

    if (!actions.isEmpty()) {
      // app.statusBar->setCurrentAction(actions.at(0).title);
    }
  }

  void hideInput() {
    app.topBar->input->hide();
    app.topBar->input->setReadOnly(true);
    app.topBar->input->setText("");
  }

  virtual void onMount() {}

public slots:
  virtual void onSearchChanged(const QString &s) {}
  virtual void onAttach() {}
  virtual void onActionActivated(ActionModel model) {}

signals:
  void launchCommand(ViewCommand *command,
                     const LaunchCommandOptions &opts = {});
  void activatePrimaryAction();
  void pushView(View *view, const PushViewOptions &options = {});
  void pop();
  void popToRoot();
};
