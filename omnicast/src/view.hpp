#pragma once
#include "app.hpp"
#include "common.hpp"
#include <qboxlayout.h>
#include <qcompare.h>
#include <qdnslookup.h>
#include <qevent.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class View : public QWidget {
  Q_OBJECT

  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::KeyPress && obj == app.topBar->input) {
      auto keyEvent = static_cast<QKeyEvent *>(event);

      return inputFilter(keyEvent);
    }

    if (event->type() == QEvent::KeyPress) { qDebug() << "keypress"; }

    return false;
  }

protected:
  AppWindow &app;

public:
  View(AppWindow &app) : QWidget(&app), app(app) {}

  void clearSearchText() { app.topBar->input->clear(); }

  void setSearchPlaceholderText(const QString &s) { app.topBar->input->setPlaceholderText(s); }

  QString searchText() { return app.topBar->input->text(); }
  QString navigationTitle() { return app.statusBar->navigationTitle(); }
  void setNavigationTitle(const QString &title) { return app.statusBar->setNavigationTitle(title); }

  QString searchText() const { return app.topBar->input->text(); }

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

  /**
   * Called when the view has been successfully pushed on top of the navigation stack.
   */
  virtual void onMount() {}

  /**
   * Called whenever the view is shown again after the view on top of it was poped.
   */
  virtual void onRestore() {}

  virtual bool inputFilter(QKeyEvent *event) { return false; }

public slots:
  virtual void onActionActivated(const AbstractAction *action) {}
  virtual void onSearchChanged(const QString &s) {}
  virtual void onAttach() {}

  /**
   * Received when the view is poped from the navigation stack, no matter the reason.
   * This is executed synchronously during the pop operation. For this reason, this function
   * should be made as fast as possible to not block the UI.
   */
  virtual void onPop() {}

signals:
  void activatePrimaryAction();
  void launchCommand(ViewCommandContext *command, const LaunchCommandOptions &opts = {});
  void pushView(View *view, const PushViewOptions &options = {});
  void setLoading(bool loading) const;
  void setSignalActions(const QList<AbstractAction *> &actions) const;
  void setActionPannel(std::vector<ActionItem> actions);
  // Pops the view from the navigation stack.
  void pop();
  void popToRoot();
};
