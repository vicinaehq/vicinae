#pragma once
#include "common.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/list-model.hpp"
#include "extension.hpp"
#include "image-viewer.hpp"
#include "markdown-renderer.hpp"
#include "tag.hpp"
#include "theme.hpp"
#include "ui/horizontal-metadata.hpp"
#include "ui/metadata-pane.hpp"
#include "ui/text-label.hpp"
#include "view.hpp"
#include <QListWidget>
#include <QTextEdit>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qobject.h>
#include <qsizepolicy.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
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
    ThemeService theme;

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

class EmptyViewWidget : public QWidget {
public:
  EmptyViewWidget(const EmptyViewModel &model) {
    auto layout = new QVBoxLayout();

    if (model.icon) {
      layout->addWidget(ImageViewer::createFromModel(*model.icon, {64, 64}));
    }

    layout->addWidget(new QLabel(model.title));
    layout->addWidget(new TextLabel(model.description));

    setLayout(layout);
  }
};

class ExtensionList : public ExtensionComponent {
  Q_OBJECT

  View &parent;
  QHBoxLayout *layout;
  QListWidget *list;
  DetailWidget *detail = nullptr;

  QList<ListChild> items;
  QHash<QListWidgetItem *, ListItemViewModel> itemMap;
  EmptyViewWidget *emptyView = nullptr;

private slots:
  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    auto &item = itemMap[current];

    qDebug() << "selected item" << item.title;

    if (item.actionPannel) {
      qDebug() << "actions:" << item.actionPannel->children.size();
      parent.setActions(*item.actionPannel);
    }

    if (item.detail) {
      auto newDetailWidget = new DetailWidget();

      qDebug() << "item has detail with"
               << item.detail->metadata.children.size() << "metadata lines";

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

    layout->setSpacing(0);
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

    qDebug() << "items" << model.items.size() << "list count" << list->count();

    items.clear();

    for (const auto &item : model.items) {
      items.push_back(item);
    }

    buildList("");

    parent.setSearchPlaceholderText(model.searchPlaceholderText);

    qDebug() << "dispatching model update";
  }

  void buildList(const QString &s = "") {
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

        // append list section name

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

    for (int i = 0; i != list->count(); ++i) {
      auto item = list->item(i);

      if (!item->flags().testFlag(Qt::ItemIsSelectable))
        continue;

      list->setCurrentItem(item);
      break;
    }
  }

  void onActionActivated(ActionModel model) {
    qDebug() << "activated" << model.title;
  }

  void onSearchTextChanged(const QString &s) {
    qDebug() << "onSearchTextChange" << model.onSearchTextChange;
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
