#include "extend/list-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

ListItemViewModel ListModelParser::parseListItem(const QJsonObject &instance, size_t index) {
  ListItemViewModel model;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();

  model.id = props["id"].toString(QString::number(index));
  model.title = props["title"].toString();
  model.subtitle = props["subtitle"].toString();
  model.icon = ImageModelParser().parse((props.value("icon").toObject()));

  size_t i = 0;

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj["type"].toString();

    if (type == "action-panel") { model.actionPannel = ActionPannelParser().parse(obj); }

    if (type == "list-item-detail") { model.detail = DetailModelParser().parse(obj); }

    i += 1;
  }

  return model;
}

ListSectionModel ListModelParser::parseSection(const QJsonObject &instance) {
  ListSectionModel model;
  auto props = instance.value("props").toObject();

  model.title = props.value("title").toString();
  model.subtitle = props.value("subtitle").toString();

  size_t index = 0;

  for (const auto &child : instance.value("children").toArray()) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();

    if (type == "list-item") {
      auto item = parseListItem(obj, index);

      model.children.push_back(item);
    }

    ++index;
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

  size_t index = 0;

  for (const auto &child : instance.value("children").toArray()) {
    auto childObj = child.toObject();
    auto type = childObj.value("type").toString();

    if (type == "list-item") {
      auto item = parseListItem(childObj, index);

      model.items.push_back(item);
    }

    if (type == "list-section") {
      auto section = parseSection(childObj);

      model.items.push_back(section);
    }

    if (type == "empty-view") { model.emptyView = EmptyViewModelParser().parse(childObj); }

    ++index;
  }

  return model;
}
