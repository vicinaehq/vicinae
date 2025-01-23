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

protected:
  VirtualListWidget *list;
  VirtualListModel *model;
  QWidget *emptyView = nullptr;
  QWidget *currentWidget = nullptr;
  QVBoxLayout *layout;

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

  virtual QWidget *createEmptyView() {
    qDebug() << "Default create empty view called";
    return nullptr;
  }

  virtual QList<AbstractAction *> createEmptyViewActions() { return {}; }

  void onListItemsChanged(const QList<std::shared_ptr<AbstractVirtualListItem>> &items) {
    if (items.isEmpty()) {
      if (!emptyView) emptyView = createEmptyView();

      if (emptyView) {
        qDebug() << "replace empty view";
        layout->replaceWidget(currentWidget, emptyView);
        currentWidget = emptyView;
      }
    }

    else if (currentWidget == emptyView) {
      layout->replaceWidget(currentWidget, list);
      currentWidget = list;
    }
  }

  NavigationListView(AppWindow &app)
      : View(app), app(app), list(new VirtualListWidget), model(new VirtualListModel),
        layout(new QVBoxLayout) {
    connect(list, &VirtualListWidget::selectionChanged, this, &NavigationListView::selectionChanged);
    connect(list, &VirtualListWidget::itemActivated, this, &NavigationListView::itemActivated);
    forwardInputEvents(list);
    list->setModel(model);

    list->setViewportMargins(10, 0, 10, 0);

    connect(model, &VirtualListModel::commitReset, this, &NavigationListView::onListItemsChanged);

    auto container = new QWidget;

    container->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);

    currentWidget = list;

    widget = container;
  }
};
