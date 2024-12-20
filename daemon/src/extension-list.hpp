#pragma once
#include "common.hpp"
#include "extension.hpp"
#include "omnicast.hpp"
#include "view.hpp"
#include <QListWidget>
#include <qboxlayout.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qnamespace.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>
#include <qnetworkrequest.h>
#include <qsizepolicy.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <variant>

class ThemeIconImageWidget : public QWidget {
  QLabel *label;

public:
  ThemeIconImageWidget(const QString &iconName, const QString &iconTheme = "")
      : label(new QLabel) {
    auto icon = QIcon::fromTheme(iconName);

    qDebug() << "icon from theme" << iconName;

    label->setPixmap(icon.pixmap(25, 25));

    auto layout = new QVBoxLayout();

    layout->addWidget(label);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
  }
};

class HttpImageWidget : public QWidget {
  Q_OBJECT
  QLabel *label;
  QNetworkAccessManager *netman;
  QSize size;

private slots:
  void requestFinished(QNetworkReply *reply) {
    QPixmap pix;

    pix.loadFromData(reply->readAll());

    auto scaledPixmap =
        pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    label->setPixmap(scaledPixmap);
  }

public:
  HttpImageWidget(const QString &url, QSize size)
      : label(new QLabel), netman(new QNetworkAccessManager), size(size) {
    QNetworkRequest req;

    connect(netman, &QNetworkAccessManager::finished, this,
            &HttpImageWidget::requestFinished);

    req.setUrl(url);
    netman->get(req);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    label->setFixedSize(size);

    setLayout(layout);
  }

  ~HttpImageWidget() { netman->deleteLater(); }
};

class FileImageWidget : public QWidget {
  QLabel *label;

public:
  FileImageWidget(const QString &path, QSize size) : label(new QLabel) {
    auto file = QFile(path);

    if (!file.exists())
      return;
    if (!file.open(QIODevice::ReadOnly))
      return;

    auto data = file.readAll();
    auto layout = new QVBoxLayout();

    label->setPixmap(QPixmap(data).scaled(size, Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label);
    setLayout(layout);
  }
};

static QWidget *createImageWidgetFromModel(const ImageLikeModel &image) {
  if (auto model = std::get_if<ImageUrlModel>(&image)) {
    return new HttpImageWidget(model->url, {25, 25});
  }

  if (auto model = std::get_if<ImageFileModel>(&image)) {
    return new FileImageWidget(model->path, {25, 25});
  }

  if (auto model = std::get_if<ThemeIconModel>(&image)) {
    return new ThemeIconImageWidget(model->iconName, model->theme);
  }

  return new ThemeIconImageWidget("application-x-executable", "");
}

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

      auto iconWidget = createImageWidgetFromModel(*item.icon);
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
