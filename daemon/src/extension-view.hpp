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

      if (auto it = props.find("actions"); it != props.end()) {
        ActionPannel pannel;
        auto obj = it->toObject();

        auto pannelProps = obj["props"].toObject();
        auto children = obj["children"].toArray();

        pannel.title = pannelProps["title"].toString();

        for (const auto &ref : children) {
          auto obj = ref.toObject();
          ActionModel action;

          action.title = obj["title"].toString();
          action.onAction = obj["onAction"].toString();
          pannel.actions.push_back(action);
        }

        itemModel.actionPannel = pannel;
      }

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
