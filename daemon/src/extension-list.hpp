#pragma once
#include "app.hpp"
#include "common.hpp"
#include "extend/detail-model.hpp"
#include "extend/list-model.hpp"
#include "extension.hpp"
#include "image-viewer.hpp"
#include "markdown-renderer.hpp"
#include "tag.hpp"
#include "theme.hpp"
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
#include <qsizepolicy.h>
#include <qtextedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <variant>

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

class MetadataWidget : public QWidget {
  QVBoxLayout *layout;

public:
  MetadataWidget() : layout(new QVBoxLayout) {
    layout->setContentsMargins(10, 10, 10, 10);
    setLayout(layout);
  }

  void addRow(QWidget *left, QWidget *right) {
    auto widget = new QWidget();
    auto rowLayout = new QHBoxLayout();

    rowLayout->setContentsMargins(0, 2, 0, 2);
    rowLayout->addWidget(left, 0, Qt::AlignLeft | Qt::AlignVCenter);
    rowLayout->addWidget(right, 0, Qt::AlignRight | Qt::AlignVCenter);
    widget->setLayout(rowLayout);
    layout->addWidget(widget);
  }

  void addSeparator() { layout->addWidget(new HDivider); }
};

class DetailWidget : public QWidget {
  QVBoxLayout *layout;
  MarkdownView *markdownEditor;
  MetadataWidget *metadata;
  Service<IconCacheService> iconCache;
  HDivider *divider;

public:
  DetailWidget(Service<IconCacheService> iconCache)
      : layout(new QVBoxLayout), markdownEditor(new MarkdownView()),
        metadata(new MetadataWidget), iconCache(iconCache),
        divider(new HDivider) {
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

    for (size_t i = 0; i != model.metadata.children.size(); ++i) {
      auto &item = model.metadata.children.at(i);

      if (auto label = std::get_if<MetadataLabel>(&item)) {
        metadata->addRow(new QLabel(label->title), new QLabel(label->text));
      }

      if (std::get_if<MetadataSeparator>(&item)) {
        metadata->addSeparator();
      }

      if (auto model = std::get_if<TagListModel>(&item)) {
        auto list = new TagList;

        for (const auto &item : model->items) {
          auto tag = new Tag();

          tag->setText(item.text);

          if (item.color) {
            tag->setColor(theme.getColor(*item.color));
          } else {
            tag->setColor(theme.getColor("primary-text"));
          }

          if (item.icon) {
            tag->addLeftWidget(
                ImageViewer::createFromModel(*item.icon, {16, 16}));
          }

          list->addTag(tag);
        }

        metadata->addRow(new QLabel(model->title), list);
      }
    }
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
  Service<IconCacheService> iconCache;
  bool isInitialChange = false;

private slots:
  void currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    auto &item = itemMap[current];

    qDebug() << "selected item" << item.title;

    if (item.actionPannel) {
      qDebug() << "actions:" << item.actionPannel->children.size();
      parent.setActions(*item.actionPannel);
    }

    if (item.detail) {
      auto newDetailWidget = new DetailWidget(iconCache);

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
        model(model), iconCache(parent.service<IconCacheService>()) {
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
      if (!item.title.contains(s, Qt::CaseInsensitive))
        continue;

      auto iconWidget = ImageViewer::createFromModel(item.icon, {25, 25});
      auto widget =
          new ListItemWidget(iconWidget, item.title, item.subtitle, "");
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
