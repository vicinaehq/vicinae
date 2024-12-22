#pragma once
#include "extension-list.hpp"
#include "extension.hpp"
#include "view.hpp"
#include <qboxlayout.h>
#include <qjsonobject.h>
#include <qtmetamacros.h>

class ExtensionView : public View {
  Q_OBJECT

  ExtensionComponent *component;
  QString componentType;
  QVBoxLayout *layout;

  void setRootComponent(const QString &type, ExtensionComponent *ui) {
    if (layout->count() > 0) {
      layout->replaceWidget(layout->itemAt(0)->widget(), ui);
    } else {
      layout->addWidget(ui);
    }

    connect(ui, &ExtensionComponent::extensionEvent, this,
            [this](const QString &action, const QJsonObject &payload) {
              emit extensionEvent(action, payload);
            });

    component = ui;
    componentType = type;
  }

  ActionModel constructActionModel(const QJsonObject &instance) {
    auto props = instance.value("props").toObject();
    ActionModel action;

    action.title = props.value("title").toString();
    action.onAction = props.value("onAction").toString();

    return action;
  }

  ActionPannelSectionModel
  constructActionPanelSection(const QJsonObject &instance) {
    auto props = instance.value("props").toObject();
    ActionPannelSectionModel model;

    for (const auto &child : instance.value("children").toArray()) {
      auto action = constructActionModel(child.toObject());

      model.actions.push_back(action);
    }

    return model;
  }

  ActionPannelSubmenuModel
  constructActionPanelSubmenu(const QJsonObject &instance) {
    auto props = instance.value("props").toObject();
    ActionPannelSubmenuModel model;

    model.title = props.value("title").toString();
    model.onOpen = props.value("onOpen").toString();
    model.onSearchTextChange = props.value("onSearchTextChange").toString();

    if (props.contains("icon")) {
      model.icon = constructImageLikeModel(props.value("icon").toObject());
    }

    for (const auto &child : instance.value("children").toArray()) {
      auto obj = child.toObject();
      auto type = obj.value("type").toString();

      if (type == "action-panel-section") {
        model.children.push_back(constructActionPanelSection(obj));
      }

      if (type == "action") {
        model.children.push_back(constructActionModel(obj));
      }
    }

    return model;
  }

  ActionPannel constructActionPannelModel(QJsonObject &instance) {
    ActionPannel pannel;
    auto props = instance["props"].toObject();
    auto children = instance["children"].toArray();

    pannel.title = props["title"].toString();

    for (const auto &ref : children) {
      auto obj = ref.toObject();
      auto type = obj.value("type").toString();

      if (type == "action") {
        pannel.children.push_back(constructActionModel(obj));
      }

      else if (type == "action-panel-section") {
        pannel.children.push_back(constructActionPanelSection(obj));
      }

      else if (type == "action-panel-submenu") {
        pannel.children.push_back(constructActionPanelSubmenu(obj));
      }
    }

    return pannel;
  }

  ColorLikeModel constructColorLikeModel(const QJsonObject &instance) {
    if (instance.contains("themeColor")) {
      return instance.value("themeColor").toString();
    }

    if (instance.contains("colorString")) {
      return instance.value("colorString").toString();
    }

    return "primary-text";
  }

  ListItemDetail constructListItemDetailModel(QJsonObject &instance) {
    ListItemDetail detail;
    auto props = instance["props"].toObject();
    auto children = instance["children"].toArray();

    detail.markdown = props["markdown"].toString();

    for (const auto &child : children) {
      auto obj = child.toObject();
      auto type = obj["type"].toString();

      if (type == "list-item-detail-metadata") {
        detail.metadata = constructListItemDetailMetadataModel(obj);
      }
    }

    return detail;
  }

  TagItemModel constructTagItemModel(const QJsonObject &instance) {
    TagItemModel model;
    auto props = instance.value("props").toObject();

    if (props.contains("icon"))
      model.icon = constructImageLikeModel(props.value("icon").toObject());

    if (props.contains("color")) {
      model.color = constructColorLikeModel(props.value("color").toObject());
    }

    model.text = props.value("text").toString();
    model.onAction = props.value("onAction").toString();

    return model;
  }

  QList<MetadataItem>
  constructListItemDetailMetadataModel(QJsonObject &instance) {
    auto children = instance["children"].toArray();
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

      if (type == "tag-list") {
        items.push_back(constructTagListModel(child));
      }
    }

    return items;
  }

  ImageLikeModel constructImageLikeModel(const QJsonObject &data) {
    ImageLikeModel model;

    qDebug() << "image" << QJsonDocument(data).toJson();

    if (data.contains("url")) {
      ImageUrlModel model;

      model.url = data.value("url").toString();

      return model;
    }

    if (data.contains("path")) {
      ImageFileModel model;

      model.path = data.value("path").toString();

      return model;
    }

    if (data.contains("iconName")) {
      ThemeIconModel model;

      model.iconName = data.value("iconName").toString();
      model.theme = data.value("theme").toString();

      return model;
    }

    return ThemeIconModel{.iconName = "application-x-executable"};
  }

  TagListModel constructTagListModel(const QJsonObject &instance) {
    TagListModel model;
    auto props = instance.value("props").toObject();

    model.title = props.value("title").toString();

    for (const auto &child : instance.value("children").toArray()) {
      model.items.push_back(constructTagItemModel(child.toObject()));
    }

    return model;
  }

  ListItemViewModel constructListItemViewModel(QJsonObject &instance) {
    ListItemViewModel model;
    auto props = instance["props"].toObject();
    auto children = instance["children"].toArray();

    model.id = props["id"].toString(
        QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));
    model.title = props["title"].toString();
    model.subtitle = props["subtitle"].toString();
    model.icon = constructImageLikeModel(props.value("icon").toObject());

    for (const auto &child : children) {
      auto obj = child.toObject();
      auto type = obj["type"].toString();

      if (type == "action-pannel") {
        model.actionPannel = constructActionPannelModel(obj);
      }

      if (type == "list-item-detail") {
        model.detail = constructListItemDetailModel(obj);
      }
    }

    return model;
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

      if (childObj["type"].toString() != "list-item") {
        throw std::runtime_error("list item can only have list-item children");
      }

      auto item = constructListItemViewModel(childObj);

      model.items.push_back(item);
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
        setRootComponent("list", new ExtensionList(model, *this));
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
  ExtensionView(AppWindow &app) : View(app) {
    widget = new QWidget();
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
  }

  ~ExtensionView() { qDebug() << "Destroy extension view"; }
};
