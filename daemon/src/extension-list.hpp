#pragma once
#include "common.hpp"
#include "extension.hpp"
#include "omnicast.hpp"
#include "view.hpp"
#include <QListWidget>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qsizepolicy.h>
#include <qwidget.h>

class DetailWidget : public QWidget {
  QVBoxLayout *layout;
  QLabel *markdown;

public:
  DetailWidget() : layout(new QVBoxLayout), markdown(new QLabel) {
    layout->addWidget(markdown);

    setLayout(layout);
  }

  void dispatchModel(const ListItemDetail &model) {
    QString s;

    for (const auto &meta : model.metadata) {
      if (auto label = std::get_if<MetadataLabel>(&meta)) {
        s += label->text;
      }
    }

    markdown->setText(s);
  }
};

class ExtensionList : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QHBoxLayout *layout;
  QListWidget *list;
  DetailWidget *detail = nullptr;

  QList<ListItemViewModel> items;
  QHash<QListWidgetItem *, ListItemViewModel> itemMap;

private slots:
  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    auto &item = itemMap[current];

    qDebug() << "selected item" << item.title;

    if (item.detail) {
      auto newDetailWidget = new DetailWidget();

      qDebug() << "item has detail with" << item.detail->metadata.size()
               << "metadata lines";

      if (layout->count() == 2) {
        layout->addWidget(newDetailWidget, 2);
      } else {
        layout->replaceWidget(detail, newDetailWidget);
        detail->deleteLater();
      }

      detail = newDetailWidget;
      detail->dispatchModel(*item.detail);
    } else if (detail) {
      layout->removeWidget(detail);
      detail->deleteLater();
    }
  }

public:
  ListModel model;

  ExtensionList(const ListModel &model, View &parent)
      : parent(parent), layout(new QHBoxLayout), list(new QListWidget()),
        model(model) {
    parent.forwardInputEvents(list);

    list->setFocusPolicy(Qt::NoFocus);
    list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(list, 1);
    layout->addWidget(new VDivider());
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
