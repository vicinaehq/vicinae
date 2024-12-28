#pragma once

#include "common.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/list-model.hpp"
#include "image-viewer.hpp"
#include "markdown-renderer.hpp"
#include "ui/action_popover.hpp"
#include "ui/empty-view.hpp"
#include "ui/horizontal-metadata.hpp"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qwidget.h>

class ListItemWidget : public QWidget {
  QWidget *icon;
  QLabel *name;
  QLabel *category;
  QLabel *kind;

public:
  ListItemWidget(QWidget *image, const QString &name, const QString &category,
                 const QString &kind, QWidget *parent = nullptr)
      : QWidget(parent), icon(image), name(new QLabel), category(new QLabel),
        kind(new QLabel) {

    auto mainLayout = new QHBoxLayout();

    setLayout(mainLayout);

    auto left = new QWidget();
    auto leftLayout = new QHBoxLayout();

    this->name->setText(name);
    this->category->setText(category);
    this->category->setProperty("class", "minor");

    left->setLayout(leftLayout);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(this->icon);
    leftLayout->addWidget(this->name);
    leftLayout->addWidget(this->category);

    mainLayout->addWidget(left, 0, Qt::AlignLeft);

    this->kind->setText(kind);
    this->kind->setProperty("class", "minor");
    mainLayout->addWidget(this->kind, 0, Qt::AlignRight);
  }
};

class DetailWidget : public QWidget {
  QVBoxLayout *layout;
  MarkdownView *markdownEditor;
  HorizontalMetadata *metadata;
  HDivider *divider;

public:
  DetailWidget()
      : layout(new QVBoxLayout), markdownEditor(new MarkdownView()),
        metadata(new HorizontalMetadata()), divider(new HDivider) {
    layout->addWidget(markdownEditor, 1);
    layout->addWidget(divider);
    layout->addWidget(metadata);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);
  }

  void dispatchModel(const DetailModel &model) {
    markdownEditor->setMarkdown(model.markdown);

    if (model.metadata.children.isEmpty()) {
      divider->hide();
      metadata->hide();
    } else {
      divider->show();
      metadata->show();
    }

    for (const auto &child : model.metadata.children) {
      metadata->addItem(child);
    }
  }
};

class ListSectionHeaderWidget : public QWidget {
public:
  ListSectionHeaderWidget(const ListSectionModel &model) {
    auto layout = new QHBoxLayout();

    auto leftWidget = new QWidget();
    auto leftLayout = new QHBoxLayout();

    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftLayout->addWidget(new TextLabel(model.title));
    leftLayout->addWidget(
        new TextLabel(QString::number(model.children.size())));
    leftWidget->setLayout(leftLayout);

    layout->addWidget(leftWidget, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(new TextLabel(model.subtitle), 0,
                      Qt::AlignRight | Qt::AlignVCenter);

    setLayout(layout);
  }
};

class ListView : public QWidget, public IInputHandler {
  Q_OBJECT

  QVBoxLayout *layout;

  QWidget *shownWidget = nullptr;

  QWidget *listWithDetails;
  QHBoxLayout *listLayout;
  QListWidget *list;
  DetailWidget *detail = nullptr;

  EmptyViewWidget *emptyView = nullptr;
  ActionPopover *actionPanel = nullptr;

  QList<ListChild> items;
  QHash<QListWidgetItem *, ListItemViewModel> itemMap;

private slots:
  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    auto &item = itemMap[current];

    emit itemChanged(item.id);

    qDebug() << "selected item" << item.title;

    if (item.actionPannel) {
      qDebug() << "actions:" << item.actionPannel->children.size();
      // actionPanel->dispatchModel(*item.actionPannel);
      emit setActions(*item.actionPannel);
    }

    if (item.detail) {
      auto newDetailWidget = new DetailWidget();

      qDebug() << "item has detail with"
               << item.detail->metadata.children.size() << "metadata lines";

      if (listLayout->count() == 2) {
        listLayout->addWidget(newDetailWidget, 2);
      } else {
        listLayout->replaceWidget(detail, newDetailWidget);
        detail->deleteLater();
      }

      detail = newDetailWidget;
      detail->dispatchModel(*item.detail);
    } else if (detail) {
      listLayout->removeWidget(detail);
      detail->deleteLater();
    }
  }

  void handleItemActivated(QListWidgetItem *listItem) {
    auto &item = itemMap[listItem];

    emit itemActivated(item.id);
  }

  void setShownWidget(QWidget *widget) {
    if (shownWidget == widget)
      return;

    widget->show();
    layout->replaceWidget(shownWidget, widget);
    shownWidget->hide();
    shownWidget = widget;
  }

  void showEmptyView(const EmptyViewModel &model) {
    auto view = new EmptyViewWidget(model);

    if (model.actions) {
      emit setActions(*model.actions);
    }

    setShownWidget(view);
  }

public:
  ListModel model;

  void handleInput(QKeyEvent *event) override {
    auto key = event->key();

    switch (key) {
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
      QApplication::sendEvent(list, event);
      break;
    default:
      break;
    }
  }

  ListView()
      : layout(new QVBoxLayout), listWithDetails(new QWidget),
        listLayout(new QHBoxLayout), list(new QListWidget()),
        actionPanel(new ActionPopover) {
    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    listLayout->setSpacing(0);
    listLayout->addWidget(list, 1);
    listLayout->addWidget(new VDivider());
    listLayout->setContentsMargins(0, 0, 0, 0);
    listWithDetails->setLayout(listLayout);

    connect(list, &QListWidget::currentItemChanged, this,
            &ListView::currentItemChanged);
    connect(list, &QListWidget::itemActivated, this,
            &ListView::handleItemActivated);

    shownWidget = listWithDetails;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(shownWidget);

    setLayout(layout);
    dispatchModel(model);
  }

  QListWidget *listWidget() { return list; }

  void dispatchModel(const ListModel &model) {
    this->model = model;

    qDebug() << "items" << model.items.size() << "list count" << list->count();

    items.clear();

    for (const auto &item : model.items) {
      items.push_back(item);
    }

    if (model.emptyView) {
      auto updatedEmptyView = new EmptyViewWidget(*model.emptyView);

      qDebug() << "refresh empty view";

      if (emptyView)
        emptyView->deleteLater();

      emptyView = updatedEmptyView;
    }

    if (!model.emptyView && emptyView) {
      emptyView->deleteLater();
      emptyView = nullptr;
    }

    filterItems("");

    qDebug() << "dispatching model update";
  }

  void filterItems(const QString &s = "") {
    list->clear();
    itemMap.clear();

    for (const auto &item : items) {
      if (auto model = std::get_if<ListSectionModel>(&item)) {
        qDebug() << "list section model with " << model->children.size();
        QList<const ListItemViewModel *> matchingItems;

        for (const auto &item : model->children) {
          if (!item.title.contains(s, Qt::CaseInsensitive))
            continue;
          matchingItems.push_back(&item);
        }

        if (matchingItems.isEmpty())
          continue;

        auto headerItem = new QListWidgetItem();

        headerItem->setFlags(headerItem->flags() & !Qt::ItemIsSelectable);
        list->addItem(headerItem);
        auto headerWidget = new ListSectionHeaderWidget(*model);
        list->setItemWidget(headerItem, headerWidget);
        headerItem->setSizeHint(headerWidget->sizeHint());

        for (const auto &item : matchingItems) {
          auto iconWidget = ImageViewer::createFromModel(item->icon, {25, 25});
          auto widget =
              new ListItemWidget(iconWidget, item->title, item->subtitle, "");
          auto listItem = new QListWidgetItem();

          list->addItem(listItem);
          list->setItemWidget(listItem, widget);
          listItem->setSizeHint(widget->sizeHint());
          itemMap.insert(listItem, *item);
        }
      }

      if (auto model = std::get_if<ListItemViewModel>(&item)) {
        if (!model->title.contains(s, Qt::CaseInsensitive))
          continue;

        auto iconWidget = ImageViewer::createFromModel(model->icon, {25, 25});
        auto widget =
            new ListItemWidget(iconWidget, model->title, model->subtitle, "");
        auto listItem = new QListWidgetItem();

        list->addItem(listItem);
        list->setItemWidget(listItem, widget);
        listItem->setSizeHint(widget->sizeHint());
        itemMap.insert(listItem, *model);
      }
    }

    qDebug() << "list count" << list->count() << model.isLoading;

    if (list->count() == 0 && !model.isLoading && model.emptyView) {
      showEmptyView(*model.emptyView);
      return;
    }

    setShownWidget(listWithDetails);

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

signals:
  void activatePrimaryAction();
  void setActions(const ActionPannelModel &model);
  void itemChanged(const QString &id);
  void itemActivated(const QString &id);
};

template <class ItemType, class ActionType> class ListController {
  QHash<QString, ItemType> itemMap;
  QHash<QString, ActionType> actionMap;

protected:
  void addItem(const QString &id, const ItemType &item) {
    itemMap.insert(id, item);
  }

  void addAction(const QString &id, const ActionType &action) {
    actionMap.insert(id, action);
  }

public:
  virtual ListModel search(const QString &s) = 0;
  virtual void onActionActivated(const ActionType &action) {
    qDebug() << "ouin ouin";
  }

  void reset() {
    itemMap.clear();
    actionMap.clear();
  }

  void handleSearch(const QString &s) { search(s); }

  void handleActionActivated(const QString &s) {
    auto &action = actionMap.value(s);

    onActionActivated(action);
  }

  ListController() {}
};
