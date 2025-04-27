#pragma once
#include "app.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qcompare.h>
#include <qevent.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class AbstractViewFactory {
  View *createView(AppWindow &app);
};

class View : public QWidget {
  Q_OBJECT
  QList<QWidget *> inputFwdTo;

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress && obj == app.topBar->input) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      if (keyEvent->key() == Qt::Key_Return) return false;

      return inputFilter(keyEvent);
    }

    if (event->type() == QEvent::KeyPress) { qDebug() << "keypress"; }

    return false;
  }

protected:
  AppWindow &app;

public:
  View(AppWindow &app) : QWidget(&app), app(app) {}
  ~View() {}

  void clearSearchText() { app.topBar->input->clear(); }

  void forwardInputEvents(QWidget *widget) {
    for (const auto &w : inputFwdTo) {
      if (widget == w) return;
    }

    inputFwdTo.push_back(widget);
  }

  template <typename T> Service<T> service() { return app.service<T>(); }
  void setSearchPlaceholderText(const QString &s) { app.topBar->input->setPlaceholderText(s); }

  QString searchText() { return app.topBar->input->text(); }
  QString navigationTitle() { return app.statusBar->navigationTitle(); }
  void setNavigationTitle(const QString &title) { return app.statusBar->setNavigationTitle(title); }

  QString searchText() const { return app.topBar->input->text(); }

  void setLoading(bool loading) { app._loadingBar->setStarted(loading); }

  void setSignalActions(const QList<AbstractAction *> &actions) {
    app.actionPannel->setSignalActions(actions);

    if (!actions.isEmpty()) {
      app.statusBar->setAction(*actions.at(0));
    } else {
      app.statusBar->clearAction();
    }
  }

  void setActionPannel(std::vector<ActionItem> actions) {
    app.actionPannel->setActions(std::move(actions));

    if (auto action = app.actionPannel->primaryAction()) {
      app.statusBar->setAction(*action);
    } else {
      app.statusBar->clearAction();
    }
  }

  void showActionPannel() {}

  void hideInput() {
    app.topBar->input->hide();
    app.topBar->input->setReadOnly(true);
    app.topBar->input->setText("");
  }

  void showInput() {
    app.topBar->input->show();
    app.topBar->input->setReadOnly(false);
  }

  virtual void onMount() {}

  // called when the view is shown again after another view that was pushed on top of it has been poped
  virtual void onRestore() {}

  virtual bool inputFilter(QKeyEvent *event) { return false; }

  virtual void onActionActivated(const AbstractAction *action) {}

public slots:
  virtual void onSearchChanged(const QString &s) {}
  virtual void onAttach() {}

signals:
  void activatePrimaryAction();
  void launchCommand(ViewCommandContext *command, const LaunchCommandOptions &opts = {});
  void pushView(View *view, const PushViewOptions &options = {});
  // Pops the view from the navigation stack.
  void pop();
  void popToRoot();
};
