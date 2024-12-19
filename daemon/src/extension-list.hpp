#pragma once
#include "extension.hpp"
#include "omnicast.hpp"
#include "view.hpp"
#include <QListWidget>
#include <qlistwidget.h>

class ExtensionList : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QListWidget *list;
  QList<ListItemViewModel> items;
  QHash<QListWidgetItem *, ListItemViewModel> itemMap;

private slots:
  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    auto &item = itemMap[current];

    qDebug() << "selected item" << item.title;

    if (item.detail) {
      qDebug() << "item has detail with" << item.detail->metadata.size()
               << "metadata lines";
    }
  }

public:
  ListModel model;

  ExtensionList(const ListModel &model, View &parent)
      : parent(parent), list(new QListWidget()), model(model) {
    auto layout = new QVBoxLayout();

    parent.forwardInputEvents(list);

    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    layout->addWidget(list);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(list, &QListWidget::currentItemChanged, this,
            &ExtensionList::currentItemChanged);

    setLayout(layout);
    dispatchModel(model);
  }

  void dispatchModel(const ListModel &model) {
    this->model = model;
    list->clear();

    parent.setSearchPlaceholderText(model.searchPlaceholderText);
    items.clear();

    for (const auto &item : model.items) {
      items.push_back(item);
    }

    buildList("");

    qDebug() << "dispatching model update";
  }

  void buildList(const QString &s = "") {
    list->clear();
    itemMap.clear();

    for (const auto &item : items) {
      if (!item.title.contains(s, Qt::CaseInsensitive))
        continue;
      auto widget =
          new GenericListItem(QIcon::fromTheme("application-x-executable"),
                              item.title, item.subtitle, "");
      auto listItem = new QListWidgetItem();

      list->addItem(listItem);
      list->setItemWidget(listItem, widget);
      listItem->setSizeHint(widget->sizeHint());
      itemMap.insert(listItem, item);
    }

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

  void onSearchTextChanged(const QString &s) {
    if (!model.onSearchTextChange.isEmpty()) {
      QJsonObject payload;
      auto args = QJsonArray();

      args.push_back(s);
      payload["args"] = args;

      emit extensionEvent(model.onSearchTextChange, payload);
    }

    if (!model.onSearchTextChange.isEmpty())
      return;

    buildList(s);
  }
};
