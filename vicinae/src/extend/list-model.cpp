#include "extend/list-model.hpp"
#include "extend/accessory-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/pagination-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlogging.h>

ImageLikeModel ListModelParser::parseListItemIcon(const QJsonValue &value) const {
  if (value.isObject()) {
    // we ignore the tooltip for now
    auto obj = value.toObject();
    if (obj.contains("value")) { return ImageModelParser().parse(obj.value("value")); }
  }

  return ImageModelParser().parse(value);
}

ListItemViewModel ListModelParser::parseListItem(const QJsonObject &instance, size_t index) {
  ListItemViewModel model;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();

  model.id = props["id"].toString(QString::number(index));
  model.title = props["title"].toString();
  model.subtitle = props["subtitle"].toString();

  if (props.contains("icon")) { model.icon = parseListItemIcon(props.value("icon")); }

  {
    auto keywords = props.value("keywords").toArray();

    model.keywords.reserve(keywords.size());
    for (const auto &value : keywords) {
      model.keywords.emplace_back(value.toString());
    }
  }

  {
    auto accessories = props.value("accessories").toArray();

    model.accessories.reserve(accessories.size());
    for (const auto &value : accessories) {
      model.accessories.emplace_back(AccessoryModel::fromJson(value));
    }
  }

  for (const auto &child : children) {
    auto obj = child.toObject();
    auto type = obj["type"].toString();

    if (type == "action-panel") { model.actionPannel = ActionPannelParser().parse(obj); }
    if (type == "list-item-detail") { model.detail = DetailModelParser().parse(obj); }
  }

  return model;
}

ListSectionModel ListModelParser::parseSection(const QJsonObject &instance) {
  ListSectionModel model;
  auto props = instance.value("props").toObject();
  auto children = instance.value("children").toArray();
  size_t index = 0;

  model.title = props.value("title").toString();
  model.subtitle = props.value("subtitle").toString();
  model.children.reserve(children.size());

  for (const auto &child : children) {
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
  // no builtin filtering by default if onSearchTextChange handler is specified
  bool defaultFiltering = !props.contains("onSearchTextChange");

  model.dirty = instance.value("dirty").toBool(false);
  model.propsDirty = instance.value("propsDirty").toBool(false);
  model.isLoading = props["isLoading"].toBool(false);
  model.throttle = props["throttle"].toBool(false);
  model.isShowingDetail = props["isShowingDetail"].toBool(false);
  model.searchPlaceholderText = props["searchBarPlaceholder"].toString();
  model.filtering = props["filtering"].toBool(defaultFiltering);

  if (props.contains("navigationTitle")) { model.navigationTitle = props["navigationTitle"].toString(); }

  if (props.contains("onSearchTextChange")) {
    model.onSearchTextChange = props.value("onSearchTextChange").toString();
  }

  if (props.contains("onSelectionChange")) {
    model.onSelectionChanged = props.value("onSelectionChange").toString();
  }

  if (props.contains("selectedItemId")) { model.selectedItemId = props.value("selectedItemId").toString(); }
  if (props.contains("searchText")) { model.searchText = props.value("searchText").toString(); }
  if (props.contains("pagination")) {
    model.pagination = PaginationModel::fromJson(props.value("pagination").toObject());
  }

  size_t index = 0;

  for (const auto &child : instance.value("children").toArray()) {
    auto childObj = child.toObject();
    auto type = childObj.value("type").toString();

    if (type == "action-panel") { model.actions = ActionPannelParser().parse(childObj); }

    if (type == "list-item") {
      auto item = parseListItem(childObj, index);

      model.items.push_back(item);
    }

    if (type == "list-section") {
      auto section = parseSection(childObj);

      model.items.push_back(section);
    }

    if (type == "dropdown") { model.searchBarAccessory = DropdownModel::fromJson(childObj); }

    if (type == "empty-view") { model.emptyView = EmptyViewModelParser().parse(childObj); }

    ++index;
  }

  return model;
}
