#include "extend/list-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

ListItemViewModel ListModelParser::parseListItem(const QJsonObject &instance) {
  ListItemViewModel model;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();

  model.id = props["id"].toString(
      QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces));
  model.title = props["title"].toString();
  model.subtitle = props["subtitle"].toString();
  model.icon = ImageModelParser().parse((props.value("icon").toObject()));

  size_t i = 0;

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj["type"].toString();

    if (type == "action-panel") {
      model.actionPannel = ActionPannelParser().parse(obj);
    }

    if (type == "list-item-detail") {
      model.detail = DetailModelParser().parse(obj);
    }

    i += 1;
  }

  return model;
}

ListModelParser::ListModelParser() {}

ListModel ListModelParser::parse(const QJsonObject &instance) {
  ListModel model;
  auto props = instance.value("props").toObject();

  model.isLoading = props["isLoading"].toBool(false);
  model.isFiltering = props["isFiltering"].toBool(false);
  model.isShowingDetail = props["isShowingDetail"].toBool(false);
  model.navigationTitle = props["navigationTitle"].toString("Command");
  model.searchPlaceholderText = props["searchBarPlaceholder"].toString();
  model.onSearchTextChange = props["onSearchTextChange"].toString();

  size_t i = 0;

  for (const auto &child : instance.value("children").toArray()) {
    auto childObj = child.toObject();

    if (childObj["type"].toString() != "list-item") {
      throw std::runtime_error("list item can only have list-item children");
    }

    auto item = parseListItem(childObj);

    model.items.push_back(item);

    /*
item.changed = patch.isSubtreeChanged(QString("/children/%1").arg(i));

if (item.changed) {
  qDebug() << "item " << i << "contents were changed";
}

i += 1;
    */
  }

  return model;
}
