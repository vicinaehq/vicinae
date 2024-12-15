#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "extension_manager.hpp"
#include "omnicast.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <variant>

class AppWindow;

class View : public QObject {
  Q_OBJECT
  AppWindow &app;

protected:
public:
  QWidget *widget;
  View(AppWindow &app) : app(app) {}

  template <typename T> Service<T> service() { return app.service<T>(); }
  void setSearchPlaceholderText(const QString &s) {
    app.topBar->input->setPlaceholderText(s);
  }

public slots:
  virtual void onSearchChanged(const QString &s) {}

signals:
  void launchCommand(ViewCommand *command);
  void pushView(View *view);
  void pop();
  void popToRoot();
};

struct MetadataLabel {
  QString text;
  QString title;
};

struct MetadataSeparator {};

using MetadataItem = std::variant<MetadataLabel, MetadataSeparator>;

struct ListItemDetail {
  QString markdown;
  QList<MetadataItem> metadata;
};

struct ListItemViewModel {
  QString id;
  QString title;
  QString subtitle;
  std::optional<ListItemDetail> detail;
};

struct ListModel {
  bool isLoading;
  bool isFiltering;
  bool isShowingDetail;
  QString navigationTitle;
  QString searchPlaceholderText;
  QList<ListItemViewModel> items;
};

class ExtensionList : public QWidget {
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
  ExtensionList(const ListModel &model, View &parent)
      : parent(parent), list(new QListWidget()) {
    auto layout = new QVBoxLayout();

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
    list->clear();

    parent.setSearchPlaceholderText(model.searchPlaceholderText);
    items.clear();

    for (const auto &item : model.items) {
      items.push_back(item);
    }

    onSearchTextChanged("");

    qDebug() << "dispatching model update";
  }

  void onSearchTextChanged(const QString &s) {
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
};

class ExtensionView : public View {
  Q_OBJECT

  Service<ExtensionManager> extensionManager;
  Service<AppDatabase> appDb;

  using RootComponent = std::variant<ExtensionList *>;

  QWidget *component;
  QString componentType;

  QVBoxLayout *layout;

  void setRootComponent(const QString &type, QWidget *ui) {
    if (layout->count() > 0) {
      layout->replaceWidget(layout->itemAt(0)->widget(), ui);
    } else {
      layout->addWidget(ui);
    }

    component = ui;
    componentType = type;
  }

  ListModel constructListModel(QJsonObject &list) {
    ListModel model;
    auto props = list["props"].toObject();

    model.isLoading = props["isLoading"].toBool(false);
    model.isFiltering = props["isFiltering"].toBool(false);
    model.isShowingDetail = props["isShowingDetail"].toBool(false);
    model.navigationTitle = props["navigationTitle"].toString("Command");
    model.searchPlaceholderText = props["searchBarPlaceholder"].toString();

    for (const auto &child : list["children"].toArray()) {
      auto childObj = child.toObject();
      ListItemViewModel itemModel;

      if (childObj["type"].toString() != "list-item") {
        throw std::runtime_error("list item can only have list-item children");
      }

      auto props = childObj["props"].toObject();

      itemModel.id = props["id"].toString(
          QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));
      itemModel.title = props["title"].toString();
      itemModel.subtitle = props["subtitle"].toString();

      if (auto it = props.find("detail"); it != props.end()) {
        auto detailProps = it->toObject()["props"].toObject();
        ListItemDetail detail;

        detail.markdown = detailProps["markdown"].toString();

        if (auto it = detailProps.find("metadata"); it != detailProps.end()) {
          auto children = it->toObject()["children"].toArray();

          QList<MetadataItem> items;

          for (const auto &ref : children) {
            auto child = ref.toObject();
            auto type = child["type"].toString();
            auto props = child["props"].toObject();

            if (type == "list-item-detail-metadata-label") {
              items.push_back(MetadataLabel{
                  .text = props["text"].toString(),
                  .title = props["title"].toString(),
              });
            }

            if (type == "list-item-detail-metadata-separator") {
              items.push_back(MetadataSeparator{});
            }
          }

          detail.metadata = items;
        }

        itemModel.detail = detail;
      }

      model.items.push_back(itemModel);
    }

    return model;
  }

  void render(QJsonObject data) {
    auto tree = data["root"].toObject();
    auto rootType = tree["type"].toString();

    if (rootType == "list") {
      auto model = constructListModel(tree);

      if (componentType != "list") {
        setRootComponent("list", new ExtensionList(model, *this));
      } else {
        static_cast<ExtensionList *>(component)->dispatchModel(model);
      }
    }
  }

public slots:
  void extensionMessage(const Message &msg) {
    if (msg.type == "list-applications") {
      QJsonObject res;
      auto appArr = QJsonArray();

      for (const auto &app : appDb.apps) {
        QJsonObject obj;

        obj["id"] = app->id;
        obj["name"] = app->name;
        appArr.push_back(obj);
      }

      res["apps"] = appArr;
      extensionManager.reply(msg, res);
    }

    if (msg.type == "render") {
      QTextStream(stdout) << QJsonDocument(msg.data).toJson();
      render(msg.data);
    }

    qDebug() << "got extension message" << msg.type << "from view";
  }

  void onSearchChanged(const QString &s) override {
    if (componentType == "list") {
      static_cast<ExtensionList *>(component)->onSearchTextChanged(s);
    }
  }

public:
  ExtensionView(AppWindow &app, const QString &name)
      : View(app), extensionManager(service<ExtensionManager>()),
        appDb(service<AppDatabase>()) {
    widget = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
  }

  ~ExtensionView() { qDebug() << "Destroy extension view"; }
};

class Command {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

/**
 */
class HeadlessCommand : public Command {
  virtual void load() = 0;
};

class ExtensionCommand : public ViewCommand {
  QString cmd;
  QString ext;

public:
  ExtensionCommand(const QString &extensionId, const QString &cmd)
      : ext(extensionId), cmd(cmd) {}

  View *load(AppWindow &app) override {
    app.extensionManager->activateCommand(ext, cmd);

    return new ExtensionView(app, cmd);
  }

  void unload(AppWindow &app) override {
    app.extensionManager->deactivateCommand(ext, cmd);
  }
};

using CommandType = std::variant<ViewCommand>;

/*
class CalculatorHistoryList : public View {
public:
  CalculatorHistoryList() { widget = new QLabel(); }

  void onSearchChanged(const QString &s) override {
    qDebug() << "on search changed";

    emit launchCommand();

    emit pushView(std::make_unique<CalculatorHistoryList>());
  }
};

class CalculatorHistoryCommand : public ViewCommand {
  UniqueView load() { return std::make_unique<CalculatorHistoryList>(); }
};
*/
