#include "extend/action-model.hpp"
#include "extend/image-model.hpp"
#include "keyboard/keybind.hpp"
#include "keyboard/keyboard.hpp"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>

static const std::unordered_map<QString, Keybind> NAMED_SHORTCUT_MAP = {
    {"copy", Keybind::CopyAction},
    {"copy-deeplink", Keybind::CopyAction},
    {"copy-name", Keybind::CopyNameAction},
    {"copy-path", Keybind::CopyPathAction},
    {"save", Keybind::SaveAction},
    {"duplicate", Keybind::DuplicateAction},
    {"edit", Keybind::EditAction},
    {"move-down", Keybind::MoveDownAction},
    {"move-up", Keybind::MoveUpAction},
    {"new", Keybind::NewAction},
    {"open", Keybind::OpenAction},
    {"open-with", Keybind::OpenAction},
    {"pin", Keybind::PinAction},
    {"refresh", Keybind::RefreshAction},
    {"remove", Keybind::RemoveAction},
    {"remove-all", Keybind::DangerousRemoveAction},
};

Keyboard::Shortcut ActionPannelParser::parseKeyboardShortcut(const QJsonValue &shortcut) {
  if (shortcut.isString()) {
    auto str = shortcut.toString();

    if (auto it = NAMED_SHORTCUT_MAP.find(str); it != NAMED_SHORTCUT_MAP.end()) { return it->second; }
    return Keyboard::Shortcut::fromString(shortcut.toString());
  }

  auto obj = shortcut.toObject();
  QStringList strs;

  strs << obj.value("key").toString();

  for (const auto &mod : obj.value("modifiers").toArray()) {
    strs << mod.toString();
  }

  return Keyboard::Shortcut::fromString(strs.join('+'));
}

ActionModel ActionPannelParser::parseAction(const QJsonObject &instance) {
  auto props = instance.value("props").toObject();
  ActionModel action;

  action.title = props.value("title").toString();
  action.onAction = props.value("onAction").toString();

  if (props.contains("onSubmit")) { action.onSubmit = props.value("onSubmit").toString(); }

  action.type = props.value("type").toString("callback");

  if (props.contains("shortcut")) { action.shortcut = parseKeyboardShortcut(props.value("shortcut")); }

  if (props.contains("icon")) { action.icon = ImageModelParser().parse(props.value("icon")); }

  if (props.contains("quicklink")) { action.quicklink = props.value("quicklink").toObject(); }

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
