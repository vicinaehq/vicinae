#pragma once
#include "app.hpp"
#include "ui/test-list.hpp"
#include "ui/virtual-list.hpp"
#include "view.hpp"

class NavigationListView : public View {
  AppWindow &app;
  VirtualListWidget *list;

protected:
  VirtualListModel *model;

public:
  void selectionChanged(const AbstractNativeListItem &item) {
    if (auto completer = item.createCompleter()) {
      app.topBar->activateQuicklinkCompleter(*completer.get());
    } else {
      app.topBar->destroyQuicklinkCompleter();
    }

    auto actions = item.createActions();
    auto size = actions.size();

    if (size > 0) actions[0]->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {}});
    if (size > 1) actions[1]->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions(actions);
  }

  NavigationListView(AppWindow &app)
      : View(app), app(app), list(new VirtualListWidget), model(new VirtualListModel) {
    connect(list, &VirtualListWidget::selectionChanged, this, &NavigationListView::selectionChanged);
    forwardInputEvents(list);
    list->setModel(model);
    widget = list;
  }
};
