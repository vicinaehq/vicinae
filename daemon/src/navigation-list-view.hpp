#pragma once
#include "app.hpp"
#include "ui/test-list.hpp"
#include "ui/virtual-list.hpp"
#include "view.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qwidget.h>

class NavigationListView : public View {
  AppWindow &app;
  // VirtualListWidget *list;
  MegaVirtualListWidget *list;

protected:
  VirtualListModel *model;

public:
  void selectionChanged(const std::shared_ptr<AbstractVirtualListItem> &listItem) {
    auto item = std::static_pointer_cast<AbstractNativeListItem>(listItem);

    if (auto completer = item->createCompleter()) {
      app.topBar->activateQuicklinkCompleter(*completer.get());
    } else {
      app.topBar->destroyQuicklinkCompleter();
    }

    auto actions = item->createActions();
    auto size = actions.size();

    if (size > 0) actions[0]->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {}});
    if (size > 1) actions[1]->setShortcut(KeyboardShortcutModel{.key = "return", .modifiers = {"ctrl"}});

    setSignalActions(actions);
  }

  void itemActivated(const std::shared_ptr<AbstractVirtualListItem> &listItem) {
    emit activatePrimaryAction();
  }

  NavigationListView(AppWindow &app)
      : View(app), app(app), list(new MegaVirtualListWidget), model(new VirtualListModel) {
    connect(list, &MegaVirtualListWidget::selectionChanged, this, &NavigationListView::selectionChanged);
    connect(list, &MegaVirtualListWidget::itemActivated, this, &NavigationListView::itemActivated);
    forwardInputEvents(list);
    list->setModel(model);

    auto container = new QWidget;
    auto containerLayout = new QVBoxLayout;

    container->setLayout(containerLayout);
    containerLayout->setContentsMargins(8, 0, 8, 0);
    containerLayout->addWidget(list);

    widget = container;
  }
};
