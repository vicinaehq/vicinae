#include "extend/grid-model.hpp"
#include "extend/action-model.hpp"
#include "extend/color-model.hpp"
#include "extend/empty-view-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "extend/pagination-model.hpp"

GridItemViewModel GridModelParser::parseGridItem(const Node &node, const NodeTree &tree, size_t index) {
  GridItemViewModel model;
  const auto &props = node.props;

  auto idStr = node_props::getStringOr(props, "id");
  model.id = idStr.empty() ? std::to_string(index) : idStr;
  model.title = node_props::getStringOr(props, "title");
  model.subtitle = node_props::getStringOr(props, "subtitle");

  if (auto *content = node_props::get(props, "content")) {
    if (content->is_object()) {
      const auto &contentObj = content->get_object();

      if (contentObj.contains("tooltip") && contentObj.at("tooltip").is_string()) {
        model.tooltip = contentObj.at("tooltip").get_string();
      }

      if (contentObj.contains("color")) {
        model.content = ColorLikeModelParser().parse(contentObj.at("color"));
      } else if (contentObj.contains("value")) {
        model.content = ImageModelParser().parse(contentObj.at("value"));
      } else {
        model.content = ImageModelParser().parse(*content);
      }
    } else {
      model.content = ImageModelParser().parse(*content);
    }
  }

  if (auto *arr = node_props::getArray(props, "keywords")) {
    model.keywords.reserve(arr->size());
    for (const auto &v : *arr) {
      if (v.is_string()) { model.keywords.emplace_back(v.get_string()); }
    }
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel") { model.actionPannel = ActionPannelParser().parse(child, tree); }
  });

  return model;
}

GridSectionModel GridModelParser::parseSection(const Node &node, const NodeTree &tree) {
  GridSectionModel model;
  size_t index = 0;
  const auto &props = node.props;

  model.title = node_props::getStringOr(props, "title");
  model.subtitle = node_props::getStringOr(props, "subtitle");

  if (auto sv = node_props::getString(props, "fit")) { model.fit = parseFit(std::string{*sv}); }
  if (node_props::has(props, "aspectRatio")) {
    model.aspectRatio = node_props::getDouble(props, "aspectRatio", 1);
  }
  if (node_props::has(props, "columns")) { model.columns = node_props::getInt(props, "columns"); }
  if (auto sv = node_props::getString(props, "inset")) { model.inset = parseInset(std::string{*sv}); }

  model.children.reserve(node.childIds.size());

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "grid-item") { model.children.push_back(parseGridItem(child, tree, index)); }
    ++index;
  });

  return model;
}

GridInset GridModelParser::parseInset(const std::string &s) {
  if (s == "zero") return GridInset::None;
  if (s == "small") return GridInset::Small;
  if (s == "medium") return GridInset::Medium;
  if (s == "large") return GridInset::Large;

  return GridInset::Small;
}

ObjectFit GridModelParser::parseFit(const std::string &fit) {
  if (fit == "fill") return ObjectFit::Fill;
  return ObjectFit::Contain;
}

GridModel GridModelParser::parse(const Node &node, const NodeTree &tree) {
  GridModel model;
  const auto &props = node.props;
  bool const defaultFiltering = !node_props::has(props, "onSearchTextChange");

  model.isLoading = node_props::getBool(props, "isLoading");
  model.throttle = node_props::getBool(props, "throttle");
  model.fit = parseFit(node_props::getStringOr(props, "fit"));
  model.aspectRatio = node_props::getDouble(props, "aspectRatio", 1);
  model.searchPlaceholderText = node_props::getStringOr(props, "searchBarPlaceholder");
  model.filtering = node_props::getBool(props, "filtering", defaultFiltering);

  if (auto sv = node_props::getString(props, "inset")) { model.inset = parseInset(std::string{*sv}); }
  if (node_props::has(props, "columns")) { model.columns = node_props::getInt(props, "columns"); }

  model.navigationTitle = node_props::getStringOr(props, "navigationTitle");

  if (auto sv = node_props::getString(props, "onSearchTextChange")) {
    model.onSearchTextChange = std::string{*sv};
  }
  if (auto sv = node_props::getString(props, "onSelectionChange")) {
    model.onSelectionChanged = std::string{*sv};
  }
  if (auto sv = node_props::getString(props, "selectedItemId")) { model.selectedItemId = std::string{*sv}; }

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

    else if (child.type == "grid-item") {
      model.items.emplace_back(parseGridItem(child, tree, index));
    }

    else if (child.type == "grid-section") {
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
