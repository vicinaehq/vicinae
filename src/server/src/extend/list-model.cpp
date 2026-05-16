#include "extend/list-model.hpp"
#include "extend/accessory-model.hpp"
#include "extend/action-model.hpp"
#include "extend/detail-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "extend/pagination-model.hpp"

ImageLikeModel ListModelParser::parseListItemIcon(const glz::generic &value) const {
  if (value.is_object()) {
    const auto &obj = value.get_object();
    if (obj.contains("value")) { return ImageModelParser().parse(obj.at("value")); }
  }

  return ImageModelParser().parse(value);
}

std::string ListModelParser::parseListItemTitle(const glz::generic &value) const {
  if (value.is_object()) {
    const auto &obj = value.get_object();
    if (obj.contains("value") && obj.at("value").is_string()) { return obj.at("value").get_string(); }
  }

  return value.is_string() ? value.get_string() : std::string{};
}

ListItemViewModel ListModelParser::parseListItem(const Node &node, const NodeTree &tree, size_t index) {
  ListItemViewModel model;
  const auto &props = node.props;

  auto idStr = node_props::getStringOr(props, "id");
  model.id = idStr.empty() ? std::to_string(index) : idStr;

  if (auto *v = node_props::get(props, "title")) { model.title = parseListItemTitle(*v); }
  if (auto *v = node_props::get(props, "subtitle")) { model.subtitle = parseListItemTitle(*v); }

  if (auto *v = node_props::get(props, "icon")) { model.icon = parseListItemIcon(*v); }

  if (auto *arr = node_props::getArray(props, "keywords")) {
    model.keywords.reserve(arr->size());
    for (const auto &value : *arr) {
      if (value.is_string()) { model.keywords.emplace_back(value.get_string()); }
    }
  }

  if (auto *arr = node_props::getArray(props, "accessories")) {
    model.accessories.reserve(arr->size());
    for (const auto &value : *arr) {
      model.accessories.emplace_back(AccessoryModel::fromGeneric(value));
    }
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel") { model.actionPannel = ActionPannelParser().parse(child, tree); }
    if (child.type == "list-item-detail") { model.detail = DetailModelParser().parse(child, tree); }
  });

  return model;
}

ListSectionModel ListModelParser::parseSection(const Node &node, const NodeTree &tree) {
  ListSectionModel model;
  const auto &props = node.props;
  size_t index = 0;

  model.title = node_props::getStringOr(props, "title");
  model.subtitle = node_props::getStringOr(props, "subtitle");
  model.children.reserve(node.childIds.size());

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "list-item") { model.children.push_back(parseListItem(child, tree, index)); }
    ++index;
  });

  return model;
}

ListModel ListModelParser::parse(const Node &node, const NodeTree &tree) {
  ListModel model;
  const auto &props = node.props;
  bool const defaultFiltering = !node_props::has(props, "onSearchTextChange");

  model.isLoading = node_props::getBool(props, "isLoading");
  model.throttle = node_props::getBool(props, "throttle");
  model.isShowingDetail = node_props::getBool(props, "isShowingDetail");
  model.searchPlaceholderText = node_props::getStringOr(props, "searchBarPlaceholder");
  model.filtering = node_props::getBool(props, "filtering", defaultFiltering);

  model.navigationTitle = node_props::getStringOr(props, "navigationTitle");

  if (auto sv = node_props::getString(props, "onSearchTextChange")) {
    model.onSearchTextChange = std::string{*sv};
  }
  if (auto sv = node_props::getString(props, "onSelectionChange")) {
    model.onSelectionChanged = std::string{*sv};
  }

  if (auto *searchTextObj = node_props::getObject(props, "searchText")) {
    auto value = node_props::getStringOr(*searchTextObj, "value");
    model.searchText = parseEventCounted(*searchTextObj, std::move(value));
  }

  if (auto *paginationObj = node_props::getObject(props, "pagination")) {
    model.pagination = PaginationModel::fromProps(*paginationObj);
  }

  size_t index = 0;

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel") {
      model.actions = ActionPannelParser().parse(child, tree);
    }

    else if (child.type == "list-item") {
      model.items.emplace_back(parseListItem(child, tree, index));
    }

    else if (child.type == "list-section") {
      model.items.emplace_back(parseSection(child, tree));
    }

    else if (child.type == "dropdown") {
      model.searchBarAccessory = DropdownModel::fromNode(child, tree);
    }

    else if (child.type == "empty-view") {
      model.emptyView = EmptyViewModelParser().parse(child, tree);
    }

    ++index;
  });

  return model;
}
