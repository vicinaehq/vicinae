#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "extend/node-props.hpp"
#include "lib/glaze-qt.hpp"

ActionModel ActionPannelParser::parseAction(const Node &node) {
  const auto &props = node.props;
  ActionModel action;

  action.title = QString::fromStdString(node_props::getStringOr(props, "title"));
  action.onAction = QString::fromStdString(node_props::getStringOr(props, "onAction"));

  if (auto sv = node_props::getString(props, "onSubmit")) {
    action.onSubmit = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  action.type = QString::fromStdString(node_props::getStringOr(props, "type", "callback"));

  if (auto *v = node_props::get(props, "shortcut")) { action.shortcut = parseKeyboardShortcut(*v); }

  if (auto *v = node_props::get(props, "icon")) { action.icon = ImageModelParser().parse(*v); }

  if (auto *v = node_props::get(props, "quicklink"); v && v->is_object()) {
    action.quicklink = glazeToQJsonObject(v->get_object());
  }

  if (auto sv = node_props::getString(props, "stableId")) {
    action.stableId = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  return action;
}

ActionPannelSubmenuPtr ActionPannelParser::parseActionPannelSubmenu(const Node &node, const NodeTree &tree) {
  const auto &props = node.props;
  auto model = std::make_shared<ActionPannelSubmenuModel>();

  model->title = QString::fromStdString(node_props::getStringOr(props, "title"));
  model->onOpen = QString::fromStdString(node_props::getStringOr(props, "onOpen"));
  model->onSearchTextChange = QString::fromStdString(node_props::getStringOr(props, "onSearchTextChange"));

  if (auto *v = node_props::get(props, "icon")) { model->icon = ImageModelParser().parse(*v); }

  if (auto *v = node_props::get(props, "shortcut")) { model->shortcut = parseKeyboardShortcut(*v); }

  if (node_props::has(props, "autoFocus")) { model->autoFocus = node_props::getBool(props, "autoFocus"); }

  if (auto *v = node_props::get(props, "filtering")) {
    if (v->is_boolean()) {
      model->filtering = v->get_boolean();
    } else if (v->is_object()) {
      const auto &filteringObj = v->get_object();
      ActionPannelSubmenuFiltering filtering;
      if (filteringObj.contains("keepSectionOrder") && filteringObj.at("keepSectionOrder").is_boolean()) {
        filtering.keepSectionOrder = filteringObj.at("keepSectionOrder").get_boolean();
      }
      model->filtering = filtering;
    }
  }

  if (node_props::has(props, "isLoading")) { model->isLoading = node_props::getBool(props, "isLoading"); }
  if (node_props::has(props, "throttle")) { model->throttle = node_props::getBool(props, "throttle"); }

  if (auto sv = node_props::getString(props, "stableId")) {
    model->stableId = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action-panel-section") {
      model->children.push_back(parseActionPannelSection(child, tree));
    } else if (child.type == "action-panel-submenu") {
      model->children.push_back(parseActionPannelSubmenu(child, tree));
    } else if (child.type == "action") {
      model->children.push_back(parseAction(child));
    }
  });

  return model;
}

ActionPannelSectionPtr ActionPannelParser::parseActionPannelSection(const Node &node, const NodeTree &tree) {
  const auto &props = node.props;
  auto model = std::make_shared<ActionPannelSectionModel>();

  if (auto sv = node_props::getString(props, "title")) {
    model->title = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action") {
      model->items.push_back(parseAction(child));
    } else if (child.type == "action-panel-submenu") {
      model->items.push_back(parseActionPannelSubmenu(child, tree));
    }
  });

  return model;
}

ActionPannelModel ActionPannelParser::parse(const Node &node, const NodeTree &tree) {
  ActionPannelModel pannel;
  const auto &props = node.props;

  pannel.title = QString::fromStdString(node_props::getStringOr(props, "title"));

  if (auto sv = node_props::getString(props, "stableId")) {
    pannel.stableId = QString::fromUtf8(sv->data(), static_cast<qsizetype>(sv->size()));
  }

  forEachChild(node, tree, [&](const Node &child) {
    if (child.type == "action") {
      pannel.children.emplace_back(parseAction(child));
    } else if (child.type == "action-panel-section") {
      pannel.children.emplace_back(parseActionPannelSection(child, tree));
    } else if (child.type == "action-panel-submenu") {
      pannel.children.emplace_back(parseActionPannelSubmenu(child, tree));
    }
  });

  return pannel;
}
