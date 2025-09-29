#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>

KeyboardShortcutModel ActionPannelParser::parseKeyboardShortcut(const QJsonObject &shortcut) {
  KeyboardShortcutModel model{.key = shortcut.value("key").toString()};

  for (const auto &mod : shortcut.value("modifiers").toArray()) {
    model.modifiers << mod.toString();
  }

  return model;
}

ActionModel ActionPannelParser::parseAction(const QJsonObject &instance) {
  auto props = instance.value("props").toObject();
  ActionModel action;

  action.title = props.value("title").toString();
  action.onAction = props.value("onAction").toString();

  if (props.contains("onSubmit")) { action.onSubmit = props.value("onSubmit").toString(); }

  auto type = props.value("type").toString("callback");

  if (props.contains("shortcut")) {
    action.shortcut = parseKeyboardShortcut(props.value("shortcut").toObject());
  }

  if (props.contains("icon")) { action.icon = ImageModelParser().parse(props.value("icon")); }

  return action;
}

ActionPannelSubmenuModel ActionPannelParser::parseActionPannelSubmenu(const QJsonObject &instance) {
  auto props = instance.value("props").toObject();
  ActionPannelSubmenuModel model;

  model.title = props.value("title").toString();
  model.onOpen = props.value("onOpen").toString();
  model.onSearchTextChange = props.value("onSearchTextChange").toString();

  if (props.contains("icon")) { model.icon = ImageModelParser().parse(props.value("icon")); }

  for (const auto &child : instance.value("children").toArray()) {
    auto obj = child.toObject();
    auto type = obj.value("type").toString();

    if (type == "action-panel-section") { model.children.push_back(parseActionPannelSection(obj)); }

    if (type == "action") { model.children.push_back(parseAction(obj)); }
  }

  return model;
}

ActionPannelSectionModel ActionPannelParser::parseActionPannelSection(const QJsonObject &instance) {
  auto props = instance.value("props").toObject();
  ActionPannelSectionModel model;

  for (const auto &child : instance.value("children").toArray()) {
    auto action = parseAction(child.toObject());

    model.actions.push_back(action);
  }

  return model;
}

ActionPannelParser::ActionPannelParser() {}

ActionPannelModel ActionPannelParser::parse(const QJsonObject &instance) {
  ActionPannelModel pannel;
  auto props = instance["props"].toObject();
  auto children = instance["children"].toArray();

  pannel.dirty = instance.value("dirty").toBool(false);
  pannel.title = props["title"].toString();

  for (const auto &ref : children) {
    auto obj = ref.toObject();
    auto type = obj.value("type").toString();

    if (type == "action") {
      pannel.children.push_back(parseAction(obj));
    }

    else if (type == "action-panel-section") {
      pannel.children.push_back(parseActionPannelSection(obj));
    }

    else if (type == "action-panel-submenu") {
      pannel.children.push_back(parseActionPannelSubmenu(obj));
    }
  }

  return pannel;
}
