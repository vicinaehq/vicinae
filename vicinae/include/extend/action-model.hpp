#pragma once
#include "extend/image-model.hpp"
#include <qjsonobject.h>
#include "lib/keyboard/keyboard.hpp"
#include <qnamespace.h>
#include <QList>
#include <memory>
#include <variant>

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
  std::optional<QString> stableId;
};

struct ActionPannelSectionModel;
struct ActionPannelSubmenuModel;

using ActionPannelSectionPtr = std::shared_ptr<ActionPannelSectionModel>;
using ActionPannelSubmenuPtr = std::shared_ptr<ActionPannelSubmenuModel>;
using ActionPannelSectionItem = std::variant<ActionModel, ActionPannelSubmenuPtr>;
using ActionPannelSubmenuChild = std::variant<ActionPannelSectionPtr, ActionModel, ActionPannelSubmenuPtr>;

struct ActionPannelSectionModel {
  QString title;
  QList<ActionPannelSectionItem> items;

  QList<ActionModel> actions() const {
    QList<ActionModel> result;
    for (const auto &item : items) {
      if (auto action = std::get_if<ActionModel>(&item)) { result.push_back(*action); }
    }
    return result;
  }
};

struct ActionPannelSubmenuFiltering {
  bool keepSectionOrder = false;
};

struct ActionPannelSubmenuModel {
  QString title;
  std::optional<ImageLikeModel> icon;
  std::optional<Keyboard::Shortcut> shortcut;
  std::optional<bool> autoFocus;
  std::optional<std::variant<bool, ActionPannelSubmenuFiltering>> filtering;
  std::optional<bool> isLoading;
  std::optional<bool> throttle;
  QString onOpen;
  QString onSearchTextChange;
  QList<ActionPannelSubmenuChild> children;
  std::optional<QString> stableId;
};

using ActionPannelItem = std::variant<ActionModel, ActionPannelSectionPtr, ActionPannelSubmenuPtr>;

struct ActionPannelModel {
  bool dirty;
  QString title;
  std::vector<ActionPannelItem> children;
  std::optional<QString> stableId;
};

class ActionPannelParser {
  Keyboard::Shortcut parseKeyboardShortcut(const QJsonValue &shortcut);
  ActionModel parseAction(const QJsonObject &instance);

  ActionPannelSectionPtr parseActionPannelSection(const QJsonObject &instance);
  ActionPannelSubmenuPtr parseActionPannelSubmenu(const QJsonObject &instance);

public:
  ActionPannelParser();
  ActionPannelModel parse(const QJsonObject &instance);
};
