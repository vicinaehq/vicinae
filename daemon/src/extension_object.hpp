#pragma once
#include "app-database.hpp"
#include "app.hpp"
#include "command-object.hpp"
#include "components/list-extension-component.hpp"
#include "extension_manager.hpp"
#include <cstring>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <stdexcept>

using RootViewModel = std::variant<ListViewModel>;
using RootComponent = std::variant<ListExtensionComponent *>;

struct InstanceType {
  QString type;
  QJsonObject props;
  QList<InstanceType> children;
};

struct JsonPatchOperation {
  enum { ADD, REPLACE, REMOVE } op;
  QString path;
  QJsonValue value;
};

using JsonPatch = QList<JsonPatchOperation>;

static InstanceType parseInstance(QJsonObject obj) {
  InstanceType instance;

  instance.type = obj["type"].toString();
  instance.props = obj["props"].toObject();

  for (const auto child : obj["children"].toArray()) {
    auto childInstance = parseInstance(child.toObject());

    instance.children.push_back(childInstance);
  }

  return instance;
}

static JsonPatch parsePatch(QJsonArray &json) {
  QList<JsonPatchOperation> ops;

  for (const auto &item : json) {
    auto obj = item.toObject();
    auto op = JsonPatchOperation::ADD;

    {
      auto opname = obj["op"];

      if (opname == "add") {
        op = JsonPatchOperation::ADD;
      } else if (opname == "replace") {
        op = JsonPatchOperation::REPLACE;
      } else if (opname == "remove") {
        op = JsonPatchOperation::REMOVE;
      }
    }

    auto path = obj["path"].toString();
    auto value = obj["value"];

    ops.push_back({op, path, value});
  }

  return ops;
}

class ExtensionObject : public CommandObject {
  Q_OBJECT

  Service<ExtensionManager> extensionManager;
  Service<AppDatabase> appDb;

  RootViewModel model;
  std::optional<RootComponent> rootComponent;

public:
  ExtensionObject(AppWindow *app, const Extension::Command &command)
      : CommandObject(app), extensionManager(service<ExtensionManager>()),
        appDb(service<AppDatabase>()) {
    extensionManager.activateCommand(command.extensionId, command.name);

    connect(&extensionManager, &ExtensionManager::extensionMessage, this,
            &ExtensionObject::extensionMessage);
  }

  void applyPatch(RootViewModel &root, JsonPatchOperation &op) {
    qDebug() << "apply patch";
    if (auto model = std::get_if<ListViewModel>(&root)) {
      applyListPatch(*model, op);
    }
  }

  void applyListPatch(ListViewModel &model,
                      const JsonPatchOperation &operation) {
    qDebug() << "apply list patch";
    QStringList paths = operation.path.split('/');

    if (paths.size() == 3 && paths[0] == "root" && paths[1] == "props") {
      auto prop = paths[2];

      if (prop == "isLoading") {
        qDebug() << "update is loading";
        model.isLoading = operation.value.toBool();
      }
      if (prop == "navigationTitle") {
        model.navigationTitle = operation.value.toString();
      }
    }

    qDebug() << operation.path;

    if (paths.size() > 3 && paths[0] == "root" && paths[1] == "children") {
      auto idx = std::stoi(paths[2].toStdString());

      if (paths.size() > 3) {
        // patch list item model specifically
      }

      if (paths.size() == 3) {
        if (operation.op == JsonPatchOperation::ADD) {
          ListItemViewModel itemModel;
          auto instance = parseInstance(operation.value.toObject());
          auto props = instance.props;

          itemModel.id = props["id"].toString();
          itemModel.title = props["title"].toString();
          itemModel.subtitle = props["subtitle"].toString();

          if (itemModel.id.isEmpty()) {
            itemModel.id = QUuid::createUuid().toString(
                QUuid::StringFormat::WithoutBraces);
          }

          model.items.push_back(itemModel);
        }

        if (operation.op == JsonPatchOperation::REMOVE) {
          model.items.removeAt(idx);
        }

        if (operation.op == JsonPatchOperation::REPLACE) {
          auto &itemModel = model.items[idx];
          auto instance = parseInstance(operation.value.toObject());
          auto props = instance.props;

          itemModel.id = props["id"].toString();
          itemModel.title = props["title"].toString();
          itemModel.subtitle = props["subtitle"].toString();

          if (itemModel.id.isEmpty()) {
            itemModel.id = QUuid::createUuid().toString(
                QUuid::StringFormat::WithoutBraces);
          }
        }
      }
    }
  }

  void render(QJsonArray arr) {
    auto patch = parsePatch(arr);
    qDebug() << "Patch has" << patch.size() << "operations";

    if (patch.isEmpty())
      return;

    auto firstOp = patch[0];

    if (firstOp.path == "/root") {

      qDebug() << "root";
      auto instance = parseInstance(firstOp.value.toObject());

      if (instance.type == "list") {
        model = ListViewModel{};
      } else {
        throw std::runtime_error("unknown model type");
      }
    }

    for (auto &op : patch) {
      applyPatch(model, op);
    }
  }

  ListViewModel constructListModel(QJsonObject &list) {
    ListViewModel model;
    auto props = list["props"].toObject();

    model.isLoading = props["isLoading"].toBool(false);
    model.isFiltering = props["isFiltering"].toBool(false);
    model.isShowingDetail = props["isShowingDetail"].toBool(false);
    model.navigationTitle = props["navigationTitle"].toString("Command");

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

      model.items.push_back(itemModel);
    }

    return model;
  }

  RootViewModel constructModel(QJsonObject tree) {
    auto rootType = tree["type"].toString();

    if (rootType == "list")
      return constructListModel(tree);

    throw std::runtime_error("unsupported root component type " +
                             rootType.toStdString());
  }

public slots:
  void extensionMessage(const Message &msg) {
    qDebug() << "extension message" << msg.type;

    if (msg.type == "list-applications") {
      QJsonObject obj;
      QJsonArray apps;

      for (const auto &app : appDb.apps) {

        if (!app->displayable())
          continue;

        QJsonObject appObj;

        appObj["id"] = app->id;
        appObj["name"] = app->name;

        apps.push_back(appObj);
      }

      obj["apps"] = apps;

      qDebug() << "sending app list";

      extensionManager.reply(msg, obj);
    }

    if (msg.type == "render") {
      // QTextStream(stdout) << QJsonDocument(msg.data).toJson();
      auto rawPatch = msg.data["patch"].toArray();
      auto renderTree = msg.data["root"].toObject();

      auto model = constructModel(renderTree);

      if (!rootComponent) {
        if (auto list = std::get_if<ListViewModel>(&model)) {
          auto listComponent = new ListExtensionComponent(app());
          widget = listComponent->widget;

          rootComponent = listComponent;
        }
      }

      if (auto listComponent =
              std::get_if<ListExtensionComponent *>(&*rootComponent)) {
        (*listComponent)->update((std::get<ListViewModel>(model)));
      }

      if (auto list = std::get_if<ListViewModel>(&model)) {
        qDebug() << "rendered list model with " << list->items.size();
      }

      // render(rawPatch);
    }
  }
};
