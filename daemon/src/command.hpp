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
  QString onSearchTextChange;
  QList<ListItemViewModel> items;
};

class ExtensionList : public QWidget {
  Q_OBJECT

  View &parent;
  QListWidget *list;
  QList<ListItemViewModel> items;
  QHash<QListWidgetItem *, ListItemViewModel> itemMap;
  Service<ExtensionManager> extensionManager;

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

  ExtensionList(const ListModel &model, View &parent,
                Service<ExtensionManager> manager)
      : parent(parent), list(new QListWidget()), extensionManager(manager),
        model(model) {
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
    this->model = model;
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

    if (!model.onSearchTextChange.isEmpty()) {
      QJsonObject payload;
      auto args = QJsonArray();

      args.push_back(s);
      payload["handlerId"] = model.onSearchTextChange;
      payload["args"] = args;
    }

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
    model.onSearchTextChange = props["onSearchTextChange"].toString();

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

public:
signals:
  void extensionEvent(const QString &action, const QJsonObject &payload);

public slots:
  void render(QJsonObject data) {
    auto tree = data["root"].toObject();
    auto rootType = tree["type"].toString();

    qDebug() << "render extension view";

    if (rootType == "list") {
      auto model = constructListModel(tree);

      if (componentType != "list") {
        setRootComponent("list",
                         new ExtensionList(model, *this, extensionManager));
      } else {
        static_cast<ExtensionList *>(component)->dispatchModel(model);
      }
    }
  }

  void onSearchChanged(const QString &s) override {
    qDebug() << "on search changed";
    if (componentType == "list") {
      static_cast<ExtensionList *>(component)->onSearchTextChanged(s);
    }
  }

public:
  ExtensionView(AppWindow &app)
      : View(app), extensionManager(service<ExtensionManager>()),
        appDb(service<AppDatabase>()) {
    widget = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
  }

  ~ExtensionView() { qDebug() << "Destroy extension view"; }
};

;

class Command : public QObject {};

class ViewCommand : public Command {
public:
  ViewCommand() {}

  virtual View *load(AppWindow &) = 0;
  virtual void unload(AppWindow &) {}

  ~ViewCommand() { qDebug() << "destroyed view"; }
};

class HeadlessCommand : public Command {
  virtual void load() = 0;
};

class ExtensionCommand : public ViewCommand {
  Q_OBJECT

  QStack<ExtensionView *> viewStack;
  QString extensionId;
  QString commandName;
  QString sessionId;
  AppWindow &app;

private slots:
  void commandLoaded(const LoadedCommand &cmd) {
    sessionId = cmd.sessionId;

    qDebug() << "Extension command loaded" << sessionId;
  }

  void extensionRequest(const QString &sessionId, const QString &id,
                        const QString &action, const QJsonObject &payload) {
    if (this->sessionId != sessionId)
      return;

    if (action == "render") {
      if (!viewStack.isEmpty()) {
        auto top = viewStack.top();

        top->render(payload);
      } else {
        auto view = new ExtensionView(app);

        viewStack.push(view);
        app.pushView(view);
        view->render(payload);
      }

      app.extensionManager->respond(id, {});
    }

    if (action == "list-applications") {
      QJsonArray apps;

      for (const auto &app : app.appDb->apps) {
        QJsonObject appObj;

        appObj["id"] = app->id;
        appObj["name"] = app->name;

        apps.push_back(appObj);
      }

      QJsonObject responseData;

      responseData["apps"] = apps;

      app.extensionManager->respond(id, responseData);
    }

    qDebug() << "extension request" << action;
  }

  void extensionEvent(const QString &sessionId, const QString &action,
                      const QJsonObject &payload) {
    if (this->sessionId != sessionId)
      return;
  }

public:
  ExtensionCommand(AppWindow &app, const QString &extensionId,
                   const QString &commandName)
      : app(app), extensionId(extensionId), commandName(commandName) {}

  View *load(AppWindow &app) override {
    connect(app.extensionManager.get(), &ExtensionManager::commandLoaded, this,
            &ExtensionCommand::commandLoaded);
    connect(app.extensionManager.get(), &ExtensionManager::extensionEvent, this,
            &ExtensionCommand::extensionEvent);
    connect(app.extensionManager.get(), &ExtensionManager::extensionRequest,
            this, &ExtensionCommand::extensionRequest);

    app.extensionManager->loadCommand(extensionId, commandName);

    return nullptr;
  }

  void unload(AppWindow &app) override {
    if (!sessionId.isEmpty())
      app.extensionManager->unloadCommand(sessionId);
  }
};
