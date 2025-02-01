#pragma once
#include "app.hpp"
#include "common.hpp"
#include "ui/test-list.hpp"
#include "ui/virtual-list.hpp"
#include "view.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qwidget.h>

class ListDetailWidget : public QWidget {
  QVBoxLayout *layout;
  QWidget *view;
  HorizontalMetadata *metadata;
  HDivider *divider;

public:
  ListDetailWidget(const AbstractNativeListItemDetail &detail)
      : layout(new QVBoxLayout), view(detail.createView()), metadata(new HorizontalMetadata()),
        divider(new HDivider) {
    layout->addWidget(view, 1);
    layout->addWidget(divider);
    layout->addWidget(metadata);
    layout->setContentsMargins(0, 0, 0, 0);

    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    metadata->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setLayout(layout);

    auto detailMetadata = detail.createMetadata();

    for (const auto &child : detailMetadata.children) {
      metadata->addItem(child);
    }
  }
};

class ListDetailSplit : public QWidget {
  QHBoxLayout *layout;
  QWidget *list;
  VDivider *divider;
  QWidget *detailWidget = nullptr;

  void resizeEvent(QResizeEvent *event) override {
    if (detailWidget) { detailWidget->setFixedWidth(width() * 0.65); }
  }

public:
  ListDetailSplit(VirtualListWidget *list) : layout(new QHBoxLayout), list(list), divider(new VDivider) {
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(list);
    layout->addWidget(divider);
    layout->setSpacing(0);

    divider->hide();

    setLayout(layout);
  }

  void clearDetail() {
    if (layout->count() == 3) {
      if (auto item = layout->takeAt(2)) { item->widget()->deleteLater(); }
    }

    detailWidget = nullptr;
  }

  void setDetail(const AbstractNativeListItemDetail &detail) {
    detailWidget = new ListDetailWidget(detail);

    if (layout->count() == 3) {
      auto currentWidget = layout->itemAt(2)->widget();

      layout->replaceWidget(currentWidget, detailWidget);
      currentWidget->deleteLater();
    } else {
      layout->addWidget(detailWidget);
    }

    detailWidget->setFixedWidth(width() * 0.65);
    divider->show();
  }
};

class NavigationListView : public View {
  AppWindow &app;

protected:
  VirtualListWidget *list;
  VirtualListModel *model;
  QWidget *emptyView = nullptr;
  QWidget *currentWidget = nullptr;
  QVBoxLayout *layout;

  ListDetailSplit *split;

public:
  void selectionChanged(const std::shared_ptr<AbstractVirtualListItem> &listItem) {
    auto item = std::static_pointer_cast<AbstractNativeListItem>(listItem);

    if (auto completer = item->createCompleter()) {
      app.topBar->activateQuicklinkCompleter(*completer.get());
    } else {
      app.topBar->destroyQuicklinkCompleter();
    }

    if (auto detail = item->createDetail()) {
      split->setDetail(*detail);
    } else {
      split->clearDetail();
    }

    auto actions = item->generateActions();
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
      app.topBar->destroyQuicklinkCompleter();
      split->clearDetail();

      if (!emptyView) emptyView = createEmptyView();

      if (emptyView) {
        layout->replaceWidget(currentWidget, emptyView);
        currentWidget = emptyView;
      }
    }

    else if (currentWidget == emptyView) {
      layout->replaceWidget(currentWidget, list);
      currentWidget = split;
    }
  }

  bool inputFilter(QKeyEvent *event) override {
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Enter:
      QApplication::sendEvent(list, event);
      return true;
    }

    return false;
  }

  NavigationListView(AppWindow &app)
      : View(app), app(app), list(new VirtualListWidget), split(new ListDetailSplit(list)),
        model(new VirtualListModel), layout(new QVBoxLayout) {
    connect(list, &VirtualListWidget::selectionChanged, this, &NavigationListView::selectionChanged);
    connect(list, &VirtualListWidget::itemActivated, this, &NavigationListView::itemActivated);
    forwardInputEvents(list);
    list->setModel(model);

    list->setViewportMargins(10, 0, 10, 0);

    connect(model, &VirtualListModel::commitReset, this, &NavigationListView::onListItemsChanged);

    auto container = new QWidget;

    container->setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(split);

    currentWidget = split;

    widget = container;
  }

  ~NavigationListView() { model->deleteLater(); }
};
