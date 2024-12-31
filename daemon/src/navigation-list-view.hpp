#pragma once
#include "app.hpp"
#include "ui/test-list.hpp"
#include "view.hpp"

class NavigationListView : public View {
  AppWindow &app;
  TestList *list;

protected:
  AbstractTestListModel *model;

public:
  void selectionChanged(const AbstractNativeListItem &item) {
    if (auto completer = item.createCompleter()) {
      app.topBar->activateQuicklinkCompleter(*completer.get());
    } else {
      app.topBar->destroyQuicklinkCompleter();
    }

    setSignalActions(item.createActions());
  }

  NavigationListView(AppWindow &app)
      : View(app), app(app), list(new TestList),
        model(new AbstractTestListModel) {
    connect(list, &TestList::selectionChanged, this,
            &NavigationListView::selectionChanged);
    forwardInputEvents(list->listWidget());
    list->setModel(model);
    widget = list;
  }
};
