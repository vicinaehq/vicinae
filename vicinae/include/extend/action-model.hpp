#pragma once
#include "extend/image-model.hpp"
#include <qjsonobject.h>
#include "lib/keyboard/keyboard.hpp"
#include <qnamespace.h>

struct KeyboardShortcutModel {
  QString key;
  QStringList modifiers;

  static KeyboardShortcutModel submit() { return {.key = "return", .modifiers = {"shift"}}; }
  static KeyboardShortcutModel remove() { return {.key = "X", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel edit() { return {.key = "E", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel removeAll() { return {.key = "X", .modifiers = {"ctrl", "shift"}}; }
  static KeyboardShortcutModel open() { return {.key = "O", .modifiers = {"ctrl"}}; }
  static KeyboardShortcutModel enter() { return {.key = "return"}; }

  bool operator==(const KeyboardShortcutModel &rhs) const {
    return key == rhs.key && modifiers == rhs.modifiers;
  }
};

struct ActionModel {
  QString title;
  QString onAction;
  std::optional<QString> onSubmit;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  QString type;
  QJsonObject quicklink;
};

struct ActionPannelSectionModel {
  QString title;
  QList<ActionModel> actions;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  QString onOpen;
  QString onSearchTextChange;
  QList<std::variant<ActionPannelSectionModel, ActionModel>> children;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionModel, ActionPannelSubmenuModel>;

struct ActionPannelModel {
  bool dirty;
  QString title;
  std::vector<ActionPannelItem> children;
};

class ActionPannelParser {
  Keyboard::Shortcut parseKeyboardShortcut(const QJsonValue &shortcut);
  ActionModel parseAction(const QJsonObject &instance);

  ActionPannelSectionModel parseActionPannelSection(const QJsonObject &instance);
  ActionPannelSubmenuModel parseActionPannelSubmenu(const QJsonObject &instance);

public:
  ActionPannelParser();
  ActionPannelModel parse(const QJsonObject &instance);
};
